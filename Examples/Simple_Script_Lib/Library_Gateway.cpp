/**
 * @file Library_Gateway.cpp
 * Contains the implementation of class Library_Gateway.
 *
 * @author Sarmad Khalid Abdulla
 *
 * @date 2013-01-03 : Created.
 */
////////////////////////////////////////////////////////////////////////////////

#include "main.h"

namespace Simple_Script
{

using namespace Core::Data;

////////////////////////////////////////////////////////////////////////////////
// Overloaded Abstract Functions

void Library_Gateway::initialize(Standard::Root_Manager *manager)
{
    Manager *grammar_manager = manager->get_grammar_manager();

    // Create execute command.
    //// execute = "execute" + Subject
    grammar_manager->set_value(STR("Execute"), Symbol_Definition::create({
        {Symbol_Def_Element::TERM, Reference_Parser::parse_qualifier(STR("root.Cmd"))},
        {Symbol_Def_Element::VARS, Map::create(false, {
                {STR("kwd"), std::make_shared<String>(STR("execute"))},
                {STR("prms"), List::create({
                    Map::create(false, {
                        {STR("prd"), Reference_Parser::parse_qualifier(STR("root.Subject"))},
                        {STR("min"), std::make_shared<Integer>(1)},
                        {STR("max"), std::make_shared<Integer>(1)},
                        {STR("pty"), std::make_shared<Integer>(1)}
                        })
                    })}
                })},
        {Symbol_Def_Element::HANDLER, this->handler}
        }));

    // Add command to leading commands list.
    List *cmd_list = this->get_leading_commands_list(grammar_manager);
    this->cmd_list_pos = static_cast<Int>(cmd_list->get_count());
    cmd_list->add(Reference_Parser::parse_qualifier(STR("root.Execute")));
}


void Library_Gateway::uninitialize(Standard::Root_Manager *manager)
{
    Manager *grammar_manager = manager->get_grammar_manager();

    // Remove command from leading commands list.
    List *cmd_list = this->get_leading_commands_list(grammar_manager);
    cmd_list->remove(this->cmd_list_pos);

    // Delete definition.
    grammar_manager->remove_value(STR("Execute"));
}


List* Library_Gateway::get_leading_commands_list(Manager *grammar_manager)
{
    Identifiable_Object *val;
    Module *module;
    grammar_manager->get_plain_value(STR("root.Main.Leading_Cmd_Grp"), val, module);
    Symbol_Definition *def = io_cast<Symbol_Definition>(val);

    if (def == 0) {
        throw General_Exception(STR("Could not find leading command group."),
                                STR("Simple_Script::Library_Gateway::get_leading_commands_list"));
    }
    Grammar_Helper helper(grammar_manager->get_interface<Provider>());
    Map *vars = helper.get_symbol_vars(def, module);
    List *cmd_list = vars->get(STR("cmds")).io_cast_get<List>();
    if (cmd_list == 0) {
        throw General_Exception(STR("Could not find leading command group's command list."),
                                STR("Simple_Script::Library_Gateway::get_leading_commands_list"));
    }
    return cmd_list;
}

} // namespace
