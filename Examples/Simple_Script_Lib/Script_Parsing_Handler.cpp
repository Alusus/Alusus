/**
 * @file Script_Parsing_Handler.cpp
 * Contains the implementation of class Script_Parsing_Handler.
 *
 * @author Sarmad Khalid Abdulla
 *
 * @date 2013-01-03 : Created.
 */
////////////////////////////////////////////////////////////////////////////////

#include "main.h"

namespace Simple_Script
{

////////////////////////////////////////////////////////////////////////////////
// Overloaded Abstract Functions

void Script_Parsing_Handler::on_prod_end(Parser::State *state)
{
    Shared_Ptr<Standard::Parsed_Item> item = state->ref_top_term_level().get_data().io_cast<Standard::Parsed_Item>();
    // Find a statement list in the subject.
    Shared_Ptr<Standard::Parsed_List> statement_list =
        Standard::Parsed_Data_Browser::get_value<Standard::Parsed_List>(STR("Subject.Subject1>Main.Statement_List"), item);
    if (statement_list != 0) {
        // Execute a list of statements.
        this->execute_list(statement_list);
    } else {
        Shared_Ptr<Standard::Parsed_Item> statement =
            Standard::Parsed_Data_Browser::get_value<Standard::Parsed_List>(STR("Subject.Subject1>0"), item);
        if (statement != 0) {
            this->execute(statement);
        } else {
            // Create a build msg.
            state->add_build_msg(
                Shared_Ptr<Common::Unrecognized_Error_Msg>(
                    new Common::Unrecognized_Error_Msg(item->get_line(), item->get_column())));
        }
    }

    // Reset parsed data because we are done with the command.
    state->ref_top_term_level().set_data(Shared_Ptr<Identifiable_Object>(0));
}


void Script_Parsing_Handler::execute(const Shared_Ptr<Standard::Parsed_Item> &statement)
{
    // Find a literal token in the subject.
    Shared_Ptr<Standard::Parsed_List> functional_exp =
        Standard::Parsed_Data_Browser::get_value<Standard::Parsed_List>(STR("Expression.Exp>Expression.Functional_Exp"),
                                                                      statement);
    if (functional_exp != 0) {
        // Execute a functional expression.
        Shared_Ptr<Standard::Parsed_Token> identifier =
            Standard::Parsed_Data_Browser::get_child_value<Standard::Parsed_Token>(STR("0>Subject.Parameter"),
                                                                                 functional_exp);
        if (ID_GENERATOR->get_desc(identifier->get_id()) == STR("IDENTIFIER_TOKEN")) {
            // Check identifier string.
            if (identifier->get_text() == STR("print")) {
                // Execute a `print` command.
                // Get the expression to print.
                // TODO: Evaluate the expression properly once expression evaluation functionality is implemented.
                Shared_Ptr<Standard::Parsed_Token> exp =
                    Standard::Parsed_Data_Browser::get_child_value<Standard::Parsed_Token>(
                        STR("1>Expression.Exp>Subject.Subject1>Subject.Literal"), functional_exp);
                std::cout << exp->get_text();
            } else if (identifier->get_text() == STR("println")) {
                // Execute a `println` command.
                // Get the expression to print.
                // TODO: Evaluate the expression properly once expression evaluation functionality is implemented.
                Shared_Ptr<Standard::Parsed_Token> exp =
                    Standard::Parsed_Data_Browser::get_child_value<Standard::Parsed_Token>(
                        STR("1>Expression.Exp>Subject.Subject1>Subject.Literal"), functional_exp);
                std::cout << exp->get_text() << std::endl;
            }
        }
    }
}

} // namespace
