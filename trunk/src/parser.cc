// cppgraph -- C++ call graph analyzer
//
//! @file parser.cc
//! @brief This file contains the implementation of the function declaration parser.
//
// Copyright (C) 2006, by Timmy <timmy1992@gmail.com>
//
// This file may be distributed under the terms of the Q Public License
// version 1.0 as appearing in the file LICENSE.QPL included in the
// packaging of this file.

#define BOOST_SPIRIT_RULE_SCANNERTYPE_LIMIT 2
#include "sys.h"
#include <boost/spirit.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <cassert>
#include "FunctionDecl.h"
#include "ClassDecl.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////////////
// Utility parsers for debugging and error handling.

namespace { // Private stuff.

using namespace boost::spirit;

#if 0
distinct_parser<> keyword_p("a-zA-Z0-9_");
#else
// This is five times faster than distinct_parser<>.
contiguous<
    sequence<
        strlit<char const*>,
	difference<
	    epsilon_parser,
	    alternative<alnum_parser, chlit<char> >
	>
    >
>
keyword_p(char const* str)
{
  return lexeme_d[str >> (eps_p - (alnum_p | ch_p('_')))];
}
#endif

//------------------------------------------------------------------------------------------------------------------------
// Functors

// Actor to clear a FunctionDecl object and mark it as constructor.
struct is_constructor_a {
  is_constructor_a(FunctionDecl& decl, std::set<std::string>& types) : M_decl(decl), M_types(types) { }
  void operator()(char const*, char const*) const { M_decl.clear(); M_types.clear(); M_decl.set_constructor(); }
  FunctionDecl& M_decl;
  std::set<std::string>& M_types;
};

// Actor to clear a FunctionDecl object and mark it as destructor.
struct is_destructor_a {
  is_destructor_a(FunctionDecl& decl, std::set<std::string>& types) : M_decl(decl), M_types(types) { }
  void operator()(char) const { M_decl.clear(); M_types.clear(); M_decl.set_destructor(); }
  FunctionDecl& M_decl;
  std::set<std::string>& M_types;
};

struct is_assignment_a {
  is_assignment_a(FunctionDecl& decl) : M_decl(decl) { }
  void operator()(char) const { M_decl.set_assignment(); }
  FunctionDecl& M_decl;
};

struct clear_output_a {
  clear_output_a(FunctionDecl& decl) : M_decl(decl) { }
  void operator()(char const*, char const*) const { M_decl.clear(); }
  FunctionDecl& M_decl;
};

struct class_name_beg_a {
  class_name_beg_a(ClassDecl& class_decl) : M_class_decl(class_decl) { }
  void operator()(char const* first, char const* last) const
      { if (M_class_decl.action_begin()) M_class_decl.set_name(std::string(first, last)); }
  ClassDecl& M_class_decl;
};

struct template_argument_list_a {
  template_argument_list_a(ClassDecl& class_decl) : M_class_decl(class_decl) { }
  void operator()(char const* first, char const* last) const
      { if (M_class_decl.action_end()) M_class_decl.set_template_argument_list(std::string(first, last)); }
  ClassDecl& M_class_decl;
};

struct class_name_end_a {
  class_name_end_a(ClassDecl& class_decl) : M_class_decl(class_decl) { }
  void operator()(char const*, char const*) const { M_class_decl.action_end(); }
  ClassDecl& M_class_decl;
};

struct nested_class_name_a {
  nested_class_name_a(std::set<std::string>& types, char const*& last) : M_types(types), M_last(last) { }
  void operator()(char const* first, char const*) const
      { if (M_last) M_types.insert(std::string(first, M_last)); M_last = NULL; }
  std::set<std::string>& M_types;
  char const*& M_last;
};

struct store_last_a {
  store_last_a(char const*& last) : M_last(last) { }
  void operator()(char const*, char const* last) const { M_last = last; }
  char const*& M_last;
};

struct erase_a {
  erase_a(std::set<std::string>& types, std::string& identifier) : M_types(types), M_identifier(identifier) { }
  void operator()(char) const
      {
        std::set<std::string>::iterator iter = M_types.find(M_identifier);
	if (iter != M_types.end())	// g++ is stupid
	  M_types.erase(iter);
      }
  std::set<std::string>& M_types;
  std::string& M_identifier;
};
 
struct returns_function_pointer_a {
  returns_function_pointer_a(FunctionDecl& decl) : M_decl(decl) { }
  void operator()(char const* first, char const* last) const
      { M_decl.function_pointer = std::string(first, last); }
  FunctionDecl& M_decl;
};

struct is_template_a {
  is_template_a(FunctionDecl& decl) : M_decl(decl) { }
  void operator()(char const*, char const*) const { M_decl.set_is_template(); }
  FunctionDecl& M_decl;
};

struct is_C_function_a {
  is_C_function_a(FunctionDecl& decl) : M_decl(decl) { }
  void operator()(char const* first, char const* last) const
      { M_decl.set_is_C_function(); M_decl.function_name = std::string(first, last); }
  FunctionDecl& M_decl;
};

struct function_scope_a {
  function_scope_a(FunctionDecl& decl) : M_decl(decl) { }
  void operator()(char const*, char const*) const;
  FunctionDecl& M_decl;
};

void function_scope_a::operator()(char const*, char const*) const
{
  std::string function_scope = M_decl.function_name;
  function_scope += '(';
  bool first = true;
  for (std::vector<std::string>::const_iterator iter = M_decl.parameter_types.begin();
       iter != M_decl.parameter_types.end(); ++iter)
  {
    if (first)
      first = false;
    else
      function_scope += ", ";
    function_scope += *iter;
  }
  function_scope += ')';
  if (!M_decl.function_qualifiers.empty())
  {
    function_scope += ' ';
    function_scope += M_decl.function_qualifiers;
  }
  if (!M_decl.exception_specification.empty())
  {
    function_scope += ' ';
    function_scope += M_decl.exception_specification;
  }
  ClassDecl class_decl(function_scope);
  class_decl.set_is_function();
  M_decl.classes.push_back(class_decl);
  M_decl.function_name.clear();
  M_decl.parameter_types.clear();
  M_decl.function_qualifiers.clear();
  M_decl.exception_specification.clear();
}

//------------------------------------------------------------------------------------------------------------------------
// Grammars

struct FUNCTIONDECLARATION : grammar<FUNCTIONDECLARATION> {
private:
  FunctionDecl& M_function_decl;
  std::set<std::string>& M_types;

public:
  FUNCTIONDECLARATION(FunctionDecl& function_decl, std::set<std::string>& types) :
      M_function_decl(function_decl), M_types(types)
  {
    BOOST_SPIRIT_DEBUG_TRACE_GRAMMAR_NAME(*this, "FUNCTIONDECLARATION", 1);
  }

private:
  mutable std::string M_last_match;
  mutable ClassDecl M_last_class;
  mutable char const* M_last;

public:
  template <typename ScannerT>
    struct definition {
      typedef rule<ScannerT> rule_t;

      rule_t abstract_declarator;
      rule_t class_key;
      rule_t class_name;
      rule_t constant_expression;
      rule_t conversion_declarator;
      rule_t conversion_type_id;
      rule_t cv_qualifier;
      rule_t cv_qualifiers;
      rule_t direct_abstract_declarator;
      rule_t elaborated_type_specifier;
      rule_t exception_specification;
      rule_t function_specifier;
      rule_t identifier;
      rule_t main;
      rule_t namespace_name;
      rule_t nested_name_specifier;
      rule_t operator_r;
      rule_t parameter_declaration;
      rule_t parameter_declaration_clause;
      rule_t ptr_operator;
      rule_t simple_type_specifier;
      rule_t simple_type_specifier_no_type;
      rule_t storage_class_specifier;
      rule_t template_argument;
      rule_t template_argument_list;
      rule_t type_id;
      rule_t type_name;
      rule_t type_specifier_no_type;
      rule_t type_specifier_type;
      rule_t type_specifier_seq;
      rule_t unqualified_id;
      rule_t decl_without_return_type;
      rule_t decl_without_return_type_no_namespace;
      rule_t with_template_parameters;
      rule_t template_parameter_assignment;
      rule_t template_argument_literal;
      rule_t conversion_operator;
      rule_t decl_specifier_no_type;
      rule_t nested_class_name;
      rule_t nested_class_name_no_template;
      rule_t nested_class_name_tail_recursion;
      rule_t nested_class_name_no_template_tail_recursion;
      rule_t unnamed;

      rule_t const& start(void) const { return main; }

      definition(FUNCTIONDECLARATION const& self)
      {
        main
	    =
  // This works because of the following:
  // 1) If we DO have a normal return type, then after consuming the
  //    type_id we look at a (possibly qualified) function name. That
  //    is, a namespace name, a class name or an unqualified name.
  //    A name never starts with a '(', nor will start with a ':'
  //    because g++ never prints a leading "::" for the root namespace.
  //    As a result, the ((ch_p('(') >> ~ch_p('*')) ^ ch_p(':')) will
  //    fail without consuming anything.  The ^ epsilon_p turns that
  //    in success and we continue with parsing decl_without_return_type
  //    as if we had done: type_id >> decl_without_return_type.
  //    Not that we make an exception for a '(' that is followed by
  //    a '*': in that case it can't be a constructor (a parameter list
  //    doesn't start with a '*') but instead we have a function that
  //    is returning a function pointer (ie: 'int (*f(void))(float)',
  //    int (*)(float) is the return type), so in that case we DO have
  //    a return type.
  //
  // 2) If we DON'T have a return type, then that MUST be a constructor
  //    or a destructor (assuming valid input). And therefore it's form
  //    will be such that in the case of a constructor, everything
  //    up till the opening parensis of the function parameter list
  //    will be eaten, while in the case of a destructor, only the
  //    first namespace (or class) will be eaten. As a result, the first
  //    remaining character in that case is either a '(' or ':', the
  //    ((ch_p('(') >> ~ch_p('*')) ^ ch_p(':')) will now succeed, and
  //    after inverting that result with ^ epsilon_p, the whole expression
  //    so far fails. The input is restored to the begin state and
  //    decl_without_return_type is called for the full input.

	    // Just an identifier means that it's a function with C linkage (extern "C").
		(
		    identifier
		    >> end_p	
		)						[is_C_function_a(self.M_function_decl)]

	    |   *decl_specifier_no_type				[assign_a(self.M_function_decl.decl_specifier)]
		>>  (
			(
			    (
			       !(
				    type_id			[assign_a(self.M_function_decl.return_type)]
				    >>  (   
					    (   
						(
						    (
						         ch_p('(')
						    >>	 ~ch_p('*')
						    )		[is_constructor_a(self.M_function_decl, self.M_types)]
						|   ch_p(':')	[is_destructor_a(self.M_function_decl, self.M_types)]
						)
						^ epsilon_p	// Invert result.
					    )
					)
				)
			    // Process the rest of the function (or all of it, when there was no return type).
			    >>  (
			            (
				        ch_p('(')
				    >>  '*'
				    >>  decl_without_return_type	// Function returning function pointer.
				    >>  (
				            ch_p(')')
				            >>  '('
				            >> !parameter_declaration_clause
				            >> ')'
				            >> !cv_qualifiers
				            >> !exception_specification
					)			[returns_function_pointer_a(self.M_function_decl)]
				    )
			        |   decl_without_return_type
			        )
			    )
			    >> !( with_template_parameters	[is_template_a(self.M_function_decl)] )
			    >> end_p
			)
		    // If that failed, there is still hope; this could be a conversion operator,
		    // which also don't have return types.  First, we have to erase the output written so far.
		    |   (   eps_p				[clear_output_a(self.M_function_decl)]
			&   nothing_p
			)
		    // Then try to parse a conversion operator.
		    |	conversion_operator
			>> !( with_template_parameters		[is_template_a(self.M_function_decl)] )
			>> end_p
		    )
	    ;

	// Function declaration without the return type.
	decl_without_return_type
	    =
		// Optional namespaces.
		// Preceding '::' are never printed by gcc, which is why
		// we can distinguish the function Foo Bar::Bar() from the constructor Foo::Bar::Bar().
		   *(   (namespace_name		[assign_a(self.M_last_match)]
			    >> "::")		[push_back_a(self.M_function_decl.class_or_namespaces, self.M_last_match)]
		    )
		    >> decl_without_return_type_no_namespace
	    ;

	// The rest of a function declaration. Any preceding scope is a class, not a namespace.
	decl_without_return_type_no_namespace
	    =
		// Optional class names.
		   *(   (class_name
			>> "::")		[push_back_a(self.M_function_decl.classes, self.M_last_class)]
		    )	

		// Function name; template parameters are not printed by g++, not even for template (member) functions.
		    >> unqualified_id		[assign_a(self.M_function_decl.function_name)]

		// Parameter list.
		    >> '('

		    >>  (

			// Just an epsilon (...).
			    str_p("...")		[push_back_a(self.M_function_decl.parameter_types)]

			// Or a comma separated list of types.

			|   (   type_id			[push_back_a(self.M_function_decl.parameter_types)]
				>> *(   ','
					>> type_id	[push_back_a(self.M_function_decl.parameter_types)]
				    )
			    // Possibly followed by an epsilon (with or without comma).
				>> !(  !ch_p(',')
					>> str_p("...")	[push_back_a(self.M_function_decl.parameter_types)]
				    )
			    )

			// Or empty parameter list ().

			|   epsilon_p

			)

		    >> ')'
		    >> !cv_qualifiers			[assign_a(self.M_function_decl.function_qualifiers)]
		    >> !exception_specification		[assign_a(self.M_function_decl.exception_specification)]
		    // If the function is followed by "::" then it wasn't the real function yet.
		    >> !(
			    // Erase parameter_types and continue parsing and recursively continue with parsing classes.
		            str_p("::")			[function_scope_a(self.M_function_decl)]
			    >> decl_without_return_type_no_namespace
			)
            ;

        conversion_operator
	    =
	        decl_without_return_type
	    ;

        with_template_parameters
	    =
	        "[with" >> template_parameter_assignment >> *(',' >> template_parameter_assignment) >> ']'
	    ;

        template_parameter_assignment
	    =
	        // First attempt to match identifier >> '=', because in by far most cases there won't be a type here.
	        identifier		[assign_a(self.M_last_match)]
		>> ch_p('=')		[erase_a(self.M_types, self.M_last_match)]
		>> type_id
	    |   type_id
	        >> ( identifier | "<anonymous>" )	[assign_a(self.M_last_match)]
		>> ch_p('=')		[erase_a(self.M_types, self.M_last_match)]
		>> template_argument_literal
	    ;

        class_name
	    =
	        // (template_id | identifier), but then smarter and storing the result.
	            identifier				[class_name_beg_a(self.M_last_class)]
		    >> !(
			    (
				(   ch_p('<')
				    >> !template_argument_list
				    >> '>'
				)   			[template_argument_list_a(self.M_last_class)]
			    // If there is no template_argument_list, mark here that we reached the end of class_name.
			    |   (   epsilon_p		[class_name_end_a(self.M_last_class)]
				// Fail, otherwise this block will eat spaces anyway.
				&   nothing_p
				)
			    )
			)
            ;

	namespace_name
	    =
	        unnamed
	    |   identifier
	    ;

        unnamed
	    =
	        str_p("<unnamed@") >> *~ch_p('>') >> ch_p('>')
            ;

	unqualified_id
	    =
	    //|   operator_function_id | conversion_function_id 
	        (keyword_p("operator") >> (operator_r | conversion_type_id))
	    |   ( '~' >> class_name )
	    //| template_id | identifier
	    |   (identifier >> !( '<' >> !template_argument_list >> '>' ))
	    ;

	operator_r
	    =
	        "=="
	    |   ch_p('=')					[is_assignment_a(self.M_function_decl)]
	    |   lexeme_d
	        [
		    (ch_p('*') | '/' | '%' | '^' | '!' | '~') >> !ch_p('=')
		|   ch_p('-') >> !(ch_p('=') | '-' | ('>' >> !ch_p('*')))
		|   ch_p('+') >> !(ch_p('=') | '+')
		|   ch_p('<') >> !(ch_p('=') | ('<' >> !ch_p('=')))
		|   ch_p('>') >> !(ch_p('=') | ('>' >> !ch_p('=')))
		|   ch_p('&') >> !(ch_p('=') | '&')
		|   ch_p('|') >> !(ch_p('=') | '|')
		]
	    |   "()"
	    |   "[]"
	    |   ','
	    |   ( keyword_p("new") | keyword_p("delete") ) >> !str_p("[]")
	    ;

	conversion_type_id
	    =   type_specifier_seq >> !conversion_declarator
	    ;

	conversion_declarator
	    =   +ptr_operator
	    ;

	template_argument_list
	    =
	        template_argument >> *( ',' >> template_argument )
            ;

	template_argument
	    =
	        type_id
	    |   template_argument_literal
	    |	"<anonymous>"
#if 0
	    // Not in function declarations.
	    |   assignment_expression
	    |   id_expression
#endif
            ;

        template_argument_literal
	    =
	        lexeme_d
		[
		    !ch_p('-')
		    >>  (
		            str_p("0x") >> hex_p 
			|   uint_p >> !(str_p("ll") | ch_p('l') | str_p("ull") | str_p("ul") | ch_p('u'))
			)
		]
	    // For enums, and booleans ("true" and "false").
	    |   identifier
	    ;

	type_id
	    =
	        type_specifier_seq >> !abstract_declarator
	    ;

	type_specifier_seq
	    =
	    // There is a problem with a sequence as normally defined: +type_specifier: it eats the
	    // function name too! The following doesn't, but it won't work for things like 'unsigned long',
	    // or even just 'short'. However, g++ always prints a trailing 'int' and therefore this
	    // will work for us.
	        *type_specifier_no_type >> type_specifier_type >> *type_specifier_no_type
	    ;

	abstract_declarator =
	        (ptr_operator >> !abstract_declarator)
	    |   direct_abstract_declarator
	    ;

	type_specifier_no_type
	    =   cv_qualifier
	    |   simple_type_specifier_no_type
	    ;

	type_specifier_type
	    =   elaborated_type_specifier
	    |   simple_type_specifier
	    ;

	simple_type_specifier_no_type
	    =   keyword_p("short")
	    |   keyword_p("long")
	    |   keyword_p("signed")
	    |   keyword_p("unsigned")
	    ;

	ptr_operator
	    =
	    // Pointer.
	        (
		// Member function pointers. Preceding "::" is not printed.
		   !(   /* !str_p("::") >> */ nested_name_specifier
		    )
                // Or normal pointer.
		    >> ch_p('*')
		// Only pointers can have qualifiers.
		    >> !cv_qualifiers
	    	    >> !keyword_p("__restrict")
		)
            // Or reference. Also a reference can have the __restrict qualifier.
	    |   ch_p('&')
	        >> !keyword_p("__restrict")
	    ;

	direct_abstract_declarator
	    =
		(   '('
		    >> abstract_declarator
		    >> ')'
		    >> *(
			    (   '('
			        >> !parameter_declaration_clause
				>> ')'
				>> !cv_qualifiers
				>> !exception_specification
			    )
			|   (   ( ch_p('[') - str_p("[with") )
			        >> constant_expression
				>> ']'
			    )
		        )
		)
	    |   (   "()"	// In the standard, this could be 'empty', but we demand that there is a (),
	                        // which is what g++ prints in that case.  This is necessary because otherwise
				// this rule could cause the return type 'type_id' to eat the entire function!
				// For example: A::A(A const&) is a type, namely: A 'function' (not a pointer
				// to function) taking (A const&) and returning the typename A::A.
				// In reality however, g++ would output that as: A::A ()(A const&).
		    >> +(
			    (   '('
				>> !parameter_declaration_clause
				>> ')'
				>> !cv_qualifiers
				>> !exception_specification
			    )
			|   (   ( ch_p('[') - str_p("[with") )
				>> constant_expression
				>> ']'
			    )
			)
	        )
	    ;

	// This is our own rule! Just eat anything but a closing square bracket.
	constant_expression
	    =   *~chset_p("[]")
	    ||  *( ( '[' >> constant_expression >> ']' ) || +~chset_p("[]") )
	    ;

	cv_qualifier
	    =   keyword_p("const")
	    |   keyword_p("volatile")
	    ;

	cv_qualifiers
	    =
	    // gcc always prints 'const' first.
	    // This matches "const", "volatile" or "const volatile".
	        keyword_p("const")
	    ||  keyword_p("volatile")
	    ;

	elaborated_type_specifier
	    =   ( class_key | keyword_p("enum") )
		>> nested_class_name_no_template
	    |   keyword_p("typename")
	        >> nested_class_name
	    ;

	nested_name_specifier
	    =
	    // class_or_namespace_name >> "::" >> (("template" >> nested_name_specifier) | !nested_name_specifier)
	    // g++ omits the "template" keyword in error messages here, so there is no need to try it.
	       +(
		    (
		        unnamed
		    |   ( identifier >> !( '<' >> !template_argument_list >> '>' ) )
		    )
		    >> "::"
		)
	    ;
        
        nested_class_name_no_template_tail_recursion
	    =
	       *(
	            ( unnamed >> "::" )
		)
		>> identifier					[store_last_a(self.M_last)]
		>>  (   ('<' >> !template_argument_list >> '>' >> "::" >> nested_class_name_no_template_tail_recursion)
	            |  !("::" >> nested_class_name_no_template_tail_recursion)
		    )
	    ;

        nested_class_name_no_template
	    =
	        nested_class_name_no_template_tail_recursion  [nested_class_name_a(self.M_types, self.M_last)]
            ;

	nested_class_name_tail_recursion
	    =  *(
	            ( unnamed >> "::" )
		)
	        >> identifier 					[store_last_a(self.M_last)]
		>> !('<' >> !template_argument_list >> '>')
		>> !("::" >> nested_class_name_tail_recursion)
	    ;

        nested_class_name
	    =
	        nested_class_name_tail_recursion	[nested_class_name_a(self.M_types, self.M_last)]
            ;

	parameter_declaration_clause
	    =   str_p("...")
	    |   (   parameter_declaration
	            >> *( ',' >> parameter_declaration )
		    >> !( !ch_p(',') >> "..." )
		)
	    ;

	parameter_declaration
	    =   *(decl_specifier_no_type | type_specifier_no_type)
	        >> type_specifier_type
		>> *type_specifier_no_type
	        >> (
	            // We don't have declarators in our function declarations.
		    // Neither do we have assignment expressions.
	                (
			 /* declarator
			|*/ !abstract_declarator
			)/* >> !( '=' >> assignment_expression )
			 */
	            )
	    ;

	exception_specification
	    =   str_p("throw")
		>> '('
		>> !(   type_id
		        >> *( ',' >> type_id )
		    )
		>> ')'
	    ;

	decl_specifier_no_type
	    =
	        storage_class_specifier
            |   function_specifier
	    |   keyword_p("friend")
	    |   keyword_p("typedef")
	    ;

	storage_class_specifier
	    =   keyword_p("auto")
	    |   keyword_p("register")
	    |   keyword_p("static")
	    |   keyword_p("extern")
	    |   keyword_p("mutable")
	    ;

	function_specifier
	    =   keyword_p("inline")
	    |   keyword_p("virtual")
	    |   keyword_p("explicit")
	    ;

	simple_type_specifier
	    =   (
		    keyword_p("char")
		|   keyword_p("wchar_t")
		|   keyword_p("bool")
		|   keyword_p("int")
		|   keyword_p("float")
		|   keyword_p("double")
	        )
		>> !keyword_p("__complex__")
	    |   keyword_p("void")
	    |   nested_class_name
	    ;


        class_key
	    =   keyword_p("class")
	    |   keyword_p("struct")
	    |   keyword_p("union")
	    ;

        type_name
	    =
	    // The last two are just identifier and will fail
	    // if class_name fails.
	        class_name
	  /*|   enum_name
	    |   typedef_name
	  */
	    ;

        identifier
	    =   lexeme_d
	        [
	            ( alpha_p | '_' ) >> *( alnum_p | '_' )
	        ]
		// We have to make sure not to think that operator is an identifier,
		// or we would think that the conversion function N::C::operator type()
		// is a normal function with return type 'N::C::operator'.
		- keyword_p("operator")
            ;

        BOOST_SPIRIT_DEBUG_TRACE_RULE_NAME(main, "FUNCTIONDECLARATION::main", 1);
	BOOST_SPIRIT_DEBUG_RULE(abstract_declarator);
	BOOST_SPIRIT_DEBUG_RULE(class_key);
	BOOST_SPIRIT_DEBUG_RULE(class_name);
	BOOST_SPIRIT_DEBUG_RULE(constant_expression);
	BOOST_SPIRIT_DEBUG_RULE(conversion_declarator);
	BOOST_SPIRIT_DEBUG_RULE(conversion_type_id);
	BOOST_SPIRIT_DEBUG_RULE(cv_qualifier);
	BOOST_SPIRIT_DEBUG_RULE(cv_qualifiers);
	BOOST_SPIRIT_DEBUG_RULE(direct_abstract_declarator);
	BOOST_SPIRIT_DEBUG_RULE(elaborated_type_specifier);
	BOOST_SPIRIT_DEBUG_RULE(exception_specification);
	BOOST_SPIRIT_DEBUG_RULE(function_specifier);
	BOOST_SPIRIT_DEBUG_RULE(identifier);
	BOOST_SPIRIT_DEBUG_RULE(namespace_name);
	BOOST_SPIRIT_DEBUG_RULE(nested_name_specifier);
	BOOST_SPIRIT_DEBUG_RULE(operator_r);
	BOOST_SPIRIT_DEBUG_RULE(parameter_declaration);
	BOOST_SPIRIT_DEBUG_RULE(parameter_declaration_clause);
	BOOST_SPIRIT_DEBUG_RULE(ptr_operator);
	BOOST_SPIRIT_DEBUG_RULE(simple_type_specifier);
	BOOST_SPIRIT_DEBUG_RULE(simple_type_specifier_no_type);
	BOOST_SPIRIT_DEBUG_RULE(storage_class_specifier);
	BOOST_SPIRIT_DEBUG_RULE(template_argument);
	BOOST_SPIRIT_DEBUG_RULE(template_argument_list);
	BOOST_SPIRIT_DEBUG_RULE(type_id);
	BOOST_SPIRIT_DEBUG_RULE(type_name);
	BOOST_SPIRIT_DEBUG_RULE(type_specifier_no_type);
	BOOST_SPIRIT_DEBUG_RULE(type_specifier_seq);
	BOOST_SPIRIT_DEBUG_RULE(unqualified_id);
	BOOST_SPIRIT_DEBUG_RULE(decl_without_return_type);
	BOOST_SPIRIT_DEBUG_RULE(decl_without_return_type_no_namespace);
	BOOST_SPIRIT_DEBUG_RULE(with_template_parameters);
	BOOST_SPIRIT_DEBUG_RULE(template_parameter_assignment);
	BOOST_SPIRIT_DEBUG_RULE(template_argument_literal);
	BOOST_SPIRIT_DEBUG_RULE(conversion_operator);
	BOOST_SPIRIT_DEBUG_RULE(type_specifier_type);
	BOOST_SPIRIT_DEBUG_RULE(decl_specifier_no_type);
	BOOST_SPIRIT_DEBUG_RULE(nested_class_name);
	BOOST_SPIRIT_DEBUG_RULE(nested_class_name_no_template);
	BOOST_SPIRIT_DEBUG_RULE(nested_class_name_tail_recursion);
	BOOST_SPIRIT_DEBUG_RULE(nested_class_name_no_template_tail_recursion);
	BOOST_SPIRIT_DEBUG_RULE(unnamed);
      }
  };

};

} // namespace, Private stuff

bool parse_function_declaration(std::string const& input, FunctionDecl& decl, std::set<std::string>& types)
{
  std::set<std::string> decl_types;
  FUNCTIONDECLARATION grammar(decl, decl_types);
  boost::spirit::parse_info<> info = boost::spirit::parse(input.c_str(), grammar, boost::spirit::space_p);
  if (info.full)
  {
    if (decl.is_assignment())
    {
      // Only keep member functions marked as 'assignment' when they take a parameter of the same type as the class.
      if (decl.parameter_types.size() != 1 || decl.self_const_reference() != decl.parameter_types.front())
        decl.unset_assignment();
    }
    else if (decl.is_constructor())
    {
      if (decl.parameter_types.size() == 1 && decl.self_const_reference() == decl.parameter_types.front())
        decl.set_copy();
      if (decl.parameter_types.size() == 0)
        decl.set_default();
    }
    // Add found types to global set of types.
    for (std::set<std::string>::iterator iter = decl_types.begin(); iter != decl_types.end(); ++iter)
    {
#ifndef CWDEBUG
      types.insert(*iter);
#else
      if (types.insert(*iter).second)
        Dout(dc::decl, "Adding type \"" << *iter << '"');
#endif
    }
  }
  return info.full;
}

#ifdef BOOST_SPIRIT_DEBUG
int main(int argc, char* argv[])
{
  Debug(debug::init());
  Debug(dc::malloc.off());

  if (argc < 2)
    return 1;

  std::string input = argv[1];

  FunctionDecl decl;
  std::set<std::string> types;

  if (!parse_function_declaration(input, decl, types))
  {
    std::cout << "Parsing failed.\n";
    return 1;
  }

  std::cout << "\nInput:\n\n" << input << '\n';
  std::cout << "\nOutput:\n\n";
  std::cout << "Types seen:\n";
  for (std::set<std::string>::iterator iter = types.begin(); iter != types.end(); ++iter)
    std::cout << "  " << *iter << '\n';
  std::string result;
  if (!decl.decl_specifier.empty())
  {
    result = decl.decl_specifier;
    result += ' ';
    std::cout << "DECL SPECIFIER     : " << decl.decl_specifier << '\n';
  }
  if (!decl.return_type.empty())
  {
    result = decl.return_type;
    result += ' ';
    std::cout << "RETURN TYPE        : " << decl.return_type;
    if (!decl.function_pointer.empty())
    {
      std::cout << " (*" << decl.function_pointer;
      result += "(* ";
    }
    std::cout << '\n';
  }
  for (std::vector<std::string>::const_iterator iter = decl.class_or_namespaces.begin();
       iter != decl.class_or_namespaces.end(); ++iter)
  {
    result += *iter;
    result += "::";
    std::cout << "CLASS OR NAMESPACE : " << *iter << "::\n";
  }
  for (std::vector<ClassDecl>::const_iterator iter = decl.classes.begin(); iter != decl.classes.end(); ++iter)
  {
    result += iter->name();
    std::cout << "CLASS NAME         : " << iter->name();
    if (iter->has_template_argument_list())
    {
      result += iter->template_argument_list();
      std::cout << "\n                   :     " << iter->template_argument_list();
    }
    std::cout << "::\n";
    result += "::";
  }
  std::cout << "FUNCTION NAME      : " << decl.function_name << "(\n";
  result += decl.function_name;
  result += '(';
  for (std::vector<std::string>::const_iterator iter = decl.parameter_types.begin(); iter != decl.parameter_types.end();)
  {
    std::vector<std::string>::const_iterator current = iter++;
    bool last = iter == decl.parameter_types.end();
    std::cout << "PARAMETER          : " << *current;
    result += *current;
    if (!last)
    {
      std::cout << ", ";
      result += ", ";
    }
    std::cout << '\n';
  }
  std::cout << "                     )\n";
  result += ')';
  if (!decl.function_qualifiers.empty())
  {
    std::cout << "QUALIFIERS         : " << decl.function_qualifiers << '\n';
    result += ' ';
    result += decl.function_qualifiers;
  }
  if (!decl.exception_specification.empty())
  {
    std::cout << "EXCEPTION SPECIFIER: " << decl.exception_specification << '\n';
    result += ' ';
    result += decl.exception_specification;
  }
  if (!decl.function_pointer.empty())
    result += decl.function_pointer;
  if (input != result)
  {
    std::cout << "\ninput  = \"" << input << "\".\n";
    std::cout << "output = \"" << result << "\".\n";
    assert(result == input);
  }
}
#endif // BOOST_SPIRIT_DEBUG

