#include "UICommand.h"
#include "webui/WebUI.h"
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>

command::UICommand &command::UICommand::getInstance() {
    static UICommand instance;
    return instance;
}

void command::UICommand::registerCommand() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand("ui", "Manager UI");
    cmd.overload().execute([]() {
            webuiServer::WebUI::getInstance().restartServer();
        });
}
