/**
 * @file Library_Gateway.h
 * Contains the header of class Library_Gateway.
 *
 * @author Sarmad Khalid Abdulla
 *
 * @date 2013-01-03 : Created.
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef LIBRARY_GATEWAY_H
#define LIBRARY_GATEWAY_H

namespace Simple_Script
{

class Library_Gateway : public Standard::Library_Gateway
{
    ////////////////////////////////////////////////////////////////////////////
    // Type Info

    TYPE_INFO(Library_Gateway, Standard::Library_Gateway, "Simple_Script", "Simple_Script", "alusus.net");


    ////////////////////////////////////////////////////////////////////////////
    // Member Variables

    private: Shared_Ptr<Script_Parsing_Handler> handler;

    private: Int cmd_list_pos;


    ////////////////////////////////////////////////////////////////////////////
    // Constructor

    public: Library_Gateway() : cmd_list_pos(-1), handler(std::make_shared<Script_Parsing_Handler>())
    {
    }

    public: virtual ~Library_Gateway()
    {
    }


    ////////////////////////////////////////////////////////////////////////////
    // Member Functions

    public: virtual void initialize(Standard::Root_Manager *manager);

    public: virtual void uninitialize(Standard::Root_Manager *manager);

    private: Data::List* get_leading_commands_list(Data::Manager *grammar_manager);

}; // class

} // namespace

#endif
