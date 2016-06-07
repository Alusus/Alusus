/**
 * @file Scg/LibraryGateway.cpp
 *
 * @copyright Copyright (C) 2014 Rafid Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <http://alusus.net/alusus_license_1_0>.
 */
//==============================================================================

#include <map>
#include <core.h>
#include <LibraryGateway.h>

namespace Scg
{

using namespace Core;
using namespace Core::Data;

//==============================================================================
// Overloaded Abstract Functions

void LibraryGateway::initialize(Standard::RootManager *manager)
{
  this->moduleHandler = std::make_shared<ModuleParsingHandler>(manager);
  this->buildHandler = std::make_shared<BuildParsingHandler>(manager);
  this->runHandler = std::make_shared<RunParsingHandler>(manager);
  this->dumpHandler = std::make_shared<DumpParsingHandler>(manager);
  this->defHandler = std::make_shared<DefParsingHandler>(manager);
  this->handler = std::make_shared<Core::Processing::Handlers::GenericParsingHandler>();

  GrammarRepository *grammarRepository = manager->getGrammarRepository();

  // Create leading commands.

  //// run = "run" + Subject
  grammarRepository->set(STR("root:Main.Run"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("run"), 0 }, { STR("نفّذ"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Subject")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->runHandler }
  }).get());

  //// build = "build" + Subject
  grammarRepository->set(STR("root:Main.Build"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("build"), 0 }, { STR("ترجم"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Subject")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->buildHandler }
  }).get());

  //// dump = "dump" + Subject
  grammarRepository->set(STR("root:Main.Dump"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), std::make_shared<String>(STR("dump")) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Subject")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->dumpHandler }
  }).get());

  //// def = "def" + Subject
  grammarRepository->set(STR("root:Main.Def"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("def"), 0 }, { STR("عرّف"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->defHandler }
  }).get());

  //// return = "return" + Subject
  grammarRepository->set(STR("root:Main.Return"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("return"), 0 }, { STR("أرجع"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression")) },
              { STR("min"), std::make_shared<Integer>(0) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());

  //// if = "if" + Exp + Statement
  grammarRepository->set(STR("root:Main.If"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("if"), 0 }, { STR("إذا"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            }),
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("module:Statement")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());

  //// for = "for" + Exp + Statement
  grammarRepository->set(STR("root:Main.For"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("for"), 0 }, { STR("لكل"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            }),
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("module:Statement")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());

  //// while = "while" + Exp + Statement
  grammarRepository->set(STR("root:Main.While"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("while"), 0 }, { STR("بينما"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            }),
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("module:Statement")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());

  //// link = "link" + Set
  grammarRepository->set(STR("root:Main.Link"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("link"), 0 }, { STR("ربط"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("module:Statement")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());

  //// Add command to leading commands list.
  SharedList *cmd_list = this->getLeadingCommandsList(grammarRepository);
  this->leadingCmdListPos = static_cast<Int>(cmd_list->getCount());
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Run")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Build")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Dump")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Def")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Return")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:If")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:For")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:While")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Link")));


  // Create inner command.

  //// module = "module" + Subject + Subject
  grammarRepository->set(STR("root:Subject.Module"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("module"), 0 }, { STR("حزمة"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:SubSet")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->moduleHandler }
  }).get());

  //// function = "function" + Exp + Statement
  // TODO: Exp and Statement need to be optional, but at least one of them is
  // given.
  /*grammarRepository->set(STR("root:Subject.Function"), SymbolDefinition::create({
    {SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd"))},
    {SymbolDefElement::VARS, SharedMap::create(false, {
    {STR("kwd"), std::make_shared<String>(STR("function"))},
    {STR("prms"), SharedList::create({
    SharedMap::create(false, {
    {STR("prd"), REF_PARSER->parseQualifier(STR("root:SubSubject"))},
    {STR("min"), std::make_shared<Integer>(1)},
    {STR("max"), std::make_shared<Integer>(2)},
    {STR("pty"), std::make_shared<Integer>(1)}
    })
    })}
    })},
    {SymbolDefElement::HANDLER, this->handler}
    }));*/
  grammarRepository->set(STR("root:Subject.Function"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("function"), 0 }, { STR("دالّة"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Expression")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            }),
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:SubSet")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());

  //// structure = "struct" + Statement
  grammarRepository->set(STR("root:Subject.Structure"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("struct"), 0 }, { STR("هيكل"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:SubSet")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());

  //// alias = "alias" + Subject
  grammarRepository->set(STR("root:Subject.Alias"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("alias"), 0 }, { STR("لقب"), 0 } }) },
        {
          STR("prms"), SharedList::create({
            SharedMap::create(false, {
              { STR("prd"), REF_PARSER->parseQualifier(STR("root:Subject")) },
              { STR("min"), std::make_shared<Integer>(1) },
              { STR("max"), std::make_shared<Integer>(1) },
              { STR("pty"), std::make_shared<Integer>(1) }
            })
          })
        }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());

  //// Add command to inner commands list.
  cmd_list = this->getInnerCommandsList(grammarRepository);
  this->innerCmdListPos = static_cast<Int>(cmd_list->getCount());
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Module")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Function")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Structure")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Alias")));

  // Create tilde commands.

  // ~cast
  grammarRepository->set(STR("root:Expression.Cast_Tilde"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("cast"), 0 }, { STR("مثّل"), 0 } }) },
        { STR("prms"), SharedList::create({}) }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());
  // ~ptr
  grammarRepository->set(STR("root:Expression.Pointer_Tilde"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("ptr"), 0 }, { STR("مؤشر"), 0 } }) },
        { STR("prms"), SharedList::create({}) }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());
  // ~cnt
  grammarRepository->set(STR("root:Expression.Content_Tilde"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("cnt"), 0 }, { STR("محتوى"), 0 } }) },
        { STR("prms"), SharedList::create({}) }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());
  // ~size
  grammarRepository->set(STR("root:Expression.Size_Tilde"), SymbolDefinition::create({
    { SymbolDefElement::TERM, REF_PARSER->parseQualifier(STR("root:Cmd")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("kwd"), SharedMap::create(false, { { STR("size"), 0 }, { STR("حجم"), 0 } }) },
        { STR("prms"), SharedList::create({}) }
      })
    },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());
  //// Add command to tilde commands list.
  cmd_list = this->getTildeCommandsList(grammarRepository);
  this->tildeCmdListPos = static_cast<Int>(cmd_list->getCount());
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Cast_Tilde")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Pointer_Tilde")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Content_Tilde")));
  cmd_list->add(REF_PARSER->parseQualifier(STR("module:Size_Tilde")));


  // Define sub statements.

  // Define a SubMain module.
  grammarRepository->set(STR("root:SubMain"), GrammarModule::create({
    { STR("@parent"), REF_PARSER->parseQualifier(STR("root:Main")) }
  }).get());

  //// def = "def" + Subject
  grammarRepository->set(STR("root:SubMain.Def"), SymbolDefinition::create({
    { SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:Def")) },
    { SymbolDefElement::HANDLER, this->handler }
  }).get());

  //// Overloaded subject.
  /*grammarRepository->set(STR("root:SubSubject"), GrammarModule::create({
    {STR("@parent"), REF_PARSER->parseQualifier(STR("root:Subject"))}
    }).get());
    grammarRepository->set(STR("root:SubSubject.Subject1"), SymbolDefinition::create({
    {SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("pmodule:Subject1"))},
    {SymbolDefElement::VARS, SharedMap::create(false, {
    {STR("sbj1"), SharedList::create({
    REF_PARSER->parseQualifier(STR("module:SubjectCmdGrp")),
    REF_PARSER->parseQualifier(STR("module:Parameter")),
    REF_PARSER->parseQualifier(STR("root:SubSet"))
    })},
    {STR("sbj2"), SharedList::create({REF_PARSER->parseQualifier(STR("root:Expression"))})},
    {STR("sbj3"), SharedList::create({REF_PARSER->parseQualifier(STR("root:Expression"))})},
    {STR("frc2"), 0},
    {STR("frc3"), 0}
    })}
    }).get());*/

  //// Overloaded set.
  grammarRepository->set(STR("root:SubSet"), SymbolDefinition::create({
    { SymbolDefElement::PARENT_REF, REF_PARSER->parseQualifier(STR("root:Set")) },
    {
      SymbolDefElement::VARS, SharedMap::create(false, {
        { STR("stmt"), REF_PARSER->parseQualifier(STR("root:SubMain.StatementList")) }
      })
    }
  }).get());
}


void LibraryGateway::uninitialize(Standard::RootManager *manager)
{
  GrammarRepository *grammarRepository = manager->getGrammarRepository();

  // Remove commands from leading commands list.
  SharedList *cmdList = this->getLeadingCommandsList(grammarRepository);

  for (int i = 0; i < 9; ++i) {
    cmdList->remove(this->leadingCmdListPos);
  }

  // Remove command from inner commands list.
  cmdList = this->getInnerCommandsList(grammarRepository);

  for (int i = 0; i < 4; ++i) {
    cmdList->remove(this->innerCmdListPos);
  }

  // Remove command from tilde commands list.
  cmdList = this->getTildeCommandsList(grammarRepository);

  for (int i = 0; i < 2; ++i) {
    cmdList->remove(this->tildeCmdListPos);
  }

  // Delete definitions.
  grammarRepository->remove(STR("root:Main.Run"));
  grammarRepository->remove(STR("root:Main.Build"));
  grammarRepository->remove(STR("root:Main.Dump"));
  grammarRepository->remove(STR("root:Main.Def"));
  grammarRepository->remove(STR("root:Main.Return"));
  grammarRepository->remove(STR("root:Main.If"));
  grammarRepository->remove(STR("root:Main.For"));
  grammarRepository->remove(STR("root:Main.While"));
  grammarRepository->remove(STR("root:Main.Link"));
  grammarRepository->remove(STR("root:Subject.Module"));
  grammarRepository->remove(STR("root:Subject.Function"));
  grammarRepository->remove(STR("root:Subject.Structure"));
  grammarRepository->remove(STR("root:Subject.Alias"));
  grammarRepository->remove(STR("root:Expression.Cast_Tilde"));
  grammarRepository->remove(STR("root:Expression.Pointer_Tilde"));
  grammarRepository->remove(STR("root:Expression.Content_Tilde"));
  grammarRepository->remove(STR("root:Expression.Size_Tilde"));
  grammarRepository->remove(STR("root:SubMain"));
  //grammarRepository->remove(STR("root:SubSubject"));
  grammarRepository->remove(STR("root:SubSet"));
}


SharedList* LibraryGateway::getLeadingCommandsList(GrammarRepository *grammarRepository)
{
  PlainPairedPtr retVal;
  grammarRepository->get(STR("root:Main.LeadingCmdGrp"), retVal.object,
                         Core::Data::Module::getTypeInfo(), &retVal.parent);
  SymbolDefinition *def = tio_cast<SymbolDefinition>(retVal.object);

  if (def == 0) {
    throw EXCEPTION(GenericException, STR("Could not find leading command group."));
  }

  GrammarContext context;
  context.setRoot(grammarRepository->getRoot().get());
  SharedMap *vars = context.getSymbolVars(def, static_cast<Core::Data::Module*>(retVal.parent));
  SharedList *cmd_list = tio_cast<SharedList>(vars->get(STR("cmds")));

  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, STR("Could not find leading command group's command list."));
  }

  return cmd_list;
}


SharedList* LibraryGateway::getInnerCommandsList(GrammarRepository *grammarRepository)
{
  PlainPairedPtr retVal;
  grammarRepository->get(STR("root:Subject.SubjectCmdGrp"), retVal.object,
                         Core::Data::Module::getTypeInfo(), &retVal.parent);
  SymbolDefinition *def = tio_cast<SymbolDefinition>(retVal.object);

  if (def == 0) {
    throw EXCEPTION(GenericException, STR("Could not find inner command group."));
  }

  GrammarContext context;
  context.setRoot(grammarRepository->getRoot().get());
  SharedMap *vars = context.getSymbolVars(def, static_cast<Core::Data::Module*>(retVal.parent));
  SharedList *cmd_list = tio_cast<SharedList>(vars->get(STR("cmds")));

  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, STR("Could not find inner command group's command list."));
  }

  return cmd_list;
}


SharedList* LibraryGateway::getTildeCommandsList(GrammarRepository *grammarRepository)
{
  PlainPairedPtr retVal;
  grammarRepository->get(STR("root:Expression.DefaultPostfixTildeCmd"), retVal.object,
                         Core::Data::Module::getTypeInfo(), &retVal.parent);
  SymbolDefinition *def = tio_cast<SymbolDefinition>(retVal.object);

  if (def == 0) {
    throw EXCEPTION(GenericException, STR("Could not find tilde command group."));
  }

  GrammarContext context;
  context.setRoot(grammarRepository->getRoot().get());
  SharedMap *vars = context.getSymbolVars(def, static_cast<Core::Data::Module*>(retVal.parent));
  SharedList *cmd_list = tio_cast<SharedList>(vars->get(STR("cmds")));

  if (cmd_list == 0) {
    throw EXCEPTION(GenericException, STR("Could not find inner command group's command list."));
  }

  return cmd_list;
}

} // namespace

