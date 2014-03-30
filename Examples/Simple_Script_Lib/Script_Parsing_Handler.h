/**
 * @file Script_Parsing_Handler.h
 * Contains the header of class Script_Parsing_Handler.
 *
 * @author Sarmad Khalid Abdulla
 *
 * @date 2013-01-03 : Created.
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef SCRIPT_PARSING_HANDLER_H
#define SCRIPT_PARSING_HANDLER_H

namespace Simple_Script
{

class Script_Parsing_Handler : public Standard::Generic_Parsing_Handler
{
    ////////////////////////////////////////////////////////////////////////////
    // Type Info

    TYPE_INFO(Script_Parsing_Handler, Standard::Generic_Parsing_Handler,
              "Simple_Script", "Simple_Script", "alusus.net");


	////////////////////////////////////////////////////////////////////////////
	// Member Variables


	////////////////////////////////////////////////////////////////////////////
	// Constructor

    public: Script_Parsing_Handler()
    {
    }


	////////////////////////////////////////////////////////////////////////////
	// Member Functions

	public: virtual void on_prod_end(Parser::State *state);

    private: void execute_list(const Shared_Ptr<Standard::Parsed_List> &list)
    {
        for (Word i = 0; i < list->get_element_count(); ++i) {
            Shared_Ptr<Standard::Parsed_Item> item = list->get_element(i).io_cast<Standard::Parsed_Item>();
            this->execute(item);
        }
    }

    private: void execute(const Shared_Ptr<Standard::Parsed_Item> &statement);

}; // class

} // namespace

#endif
