#include "Player.hpp"

#include <memory>

#include <plog/Log.h>

#include <IGame.h>
#include <IScriptEngine.h>
#include <Chat.h>
#include <Network/Connection.hpp>
#include <World/World.hpp>
#include <World/Tile.hpp>
#include <World/Objects.hpp>
#include <World/Map.hpp>
#include <ClientUI/WelcomeWindowSink.h>

#include <Shared/ErrorHandling.h>
#include <Shared/Network/Protocol/ClientToServer/Commands.h>

class Server;

using namespace std::string_literals;

Player::Player(std::string ckey) : ckey(ckey) {
	control = nullptr;
	AddVerb("spawn", &Player::OpenSpawnWindow);
}

void Player::SetConnection(sptr<Connection> &connection) {
	this->connection = connection;
	if (camera && control) {
		camera->SetPosition(control->GetOwner()->GetTile());
	}
}

void Player::JoinToGame() {
	actions.Push(new JoinPlayerCommand);
}

void Player::ChatMessage(std::string &message) {
	GGame->GetChat()->AddMessage("<" + ckey + ">" + message);
}

void Player::Move(uf::Direction direction) {
	actions.Push(new MovePlayerCommand(uf::DirectionToVect(direction)));
}

void Player::MoveZ(bool up) {
	actions.Push(new MoveZPlayerCommand(up));
}

void Player::ClickObject(uint id) {
    actions.Push(new ClickObjectPlayerCommand(id));
}

void Player::ClickControlUI(const std::string &id) {
	actions.Push(new ClickControlUIPlayerCommand(id));
}

void Player::CallVerb(const std::string &verb) {
	actions.Push(new VerbPlayerCommand(verb));
}

void Player::UIInput(uptr<network::protocol::UIData> &&data) {
	auto iter = uiSinks.find(data->window);
	if (iter != uiSinks.end())
		iter->second->OnInput(data->handle, std::forward<std::unique_ptr<network::protocol::UIData>>(data));
}

void Player::UITrigger(const std::string &window, const std::string &trigger) {
	auto iter = uiSinks.find(window);
	if (iter != uiSinks.end())
		iter->second->OnTrigger(trigger);
}

void Player::SpawnWindowSearchCommand(const std::string &searchBuffer) {
	auto types = GGame->GetScriptEngine()->GetTypesInfo(searchBuffer);
	auto command = std::make_unique<network::protocol::server::UpdateSpawnWindowCommand>();

	for (auto type: types)
		command->types.push_back(*type);

	AddCommandToClient(command.release());
}

void Player::SpawnWindowSpawnCommand(const std::string &typeKey) {
	if (control) {
		auto *obj = GGame->GetScriptEngine()->CreateObjectByKey(typeKey);
		if (obj)
			obj->SetTile(control->GetOwner()->GetTile());
	}
}

void Player::updateUISinks(std::chrono::microseconds timeElapsed) {
	for (auto iter = uiSinks.begin(); iter != uiSinks.end();) {
		auto *sink = iter->second.get();
		sink->Update(timeElapsed);
		if (sink->IsClosed())
			iter = uiSinks.erase(iter);
		else
			iter++;
	}
}

void Player::Update(std::chrono::microseconds timeElapsed) {
	while (!syncCommands.Empty()) {
		auto generalCommand = syncCommands.Pop();

		using namespace network::protocol;

		if (auto *command = dynamic_cast<client::SpawnWindowSearchCommand *>(generalCommand.get())) {
			SpawnWindowSearchCommand(command->searchBuffer);
			continue;
		}

		if (auto *command = dynamic_cast<client::SpawnWindowSpawnCommand *>(generalCommand.get())) {
			SpawnWindowSpawnCommand(command->typeKey);
			continue;
		}

		LOGE << "Unknown command (ser id is 0x" << std::hex << generalCommand->Id() << ") was not processed as synced! "
			<< "Player: " << ckey;
	}

    while (!actions.Empty()) {
        PlayerCommand *temp = actions.Pop();
        if (temp) {
            switch (temp->GetCode()) {
                case PlayerCommand::Code::JOIN: {
                    SetControl(GGame->GetStartControl(this));
					verbsHolders["player"] = this;
					verbsHolders["atmos"] = GetControl()->GetOwner()->GetTile()->GetMap()->GetAtmos();
					verbsHolders["creature"] = GetControl()->GetOwner();
                    break;
                }
				case PlayerCommand::Code::MOVE: {
					if (control) {
						MovePlayerCommand *moveCommand = dynamic_cast<MovePlayerCommand *>(temp);
						control->MoveCommand(moveCommand->order);
					}
					break;
				}
				case PlayerCommand::Code::MOVEZ: {
					if (control) {
						MoveZPlayerCommand *moveZCommand = dynamic_cast<MoveZPlayerCommand *>(temp);
						control->MoveZCommand(moveZCommand->order);
					}
					break;
				}
                case PlayerCommand::Code::CLICK_OBJECT: {
                    if (control) {
                        auto clickObjectPlayerCommand = dynamic_cast<ClickObjectPlayerCommand *>(temp);
                        control->ClickObjectCommand(clickObjectPlayerCommand->id);
                    }
                    break;
                }
				case PlayerCommand::Code::CLICK_CONTROL_UI: {
					if (control) {
						auto clickControlUIPlayerCommand = dynamic_cast<ClickControlUIPlayerCommand *>(temp);
						control->ClickUICommand(clickControlUIPlayerCommand->id);
					}
					break;
				}
				case PlayerCommand::Code::VERB: {
					auto verbPlayerCommand = dynamic_cast<VerbPlayerCommand *>(temp);
					auto &verb = verbPlayerCommand->verb;
					try {
						auto delimiter = verb.find(".");
						EXPECT(delimiter != verb.npos);
						std::string verbHolder = verb.substr(0, delimiter);
						std::string verbName = verb.substr(delimiter + 1);

						auto nameAndVerbHolder = verbsHolders.find(verbHolder);
						EXPECT_WITH_MSG(nameAndVerbHolder != verbsHolders.end(), "VerbHolder \""s + verbHolder + "\" doesn't exist!");

						nameAndVerbHolder->second->CallVerb(this, verbName);
					} catch (const std::exception &e) {
						MANAGE_EXCEPTION(e);
					}
				}
                default:
                    break;
            }
            delete temp;
        }
    }

	camera->Update(timeElapsed);
	updateUISinks(timeElapsed);
}

void Player::SendGraphicsUpdates(std::chrono::microseconds timeElapsed) {
	if (camera)
		camera->UpdateView(timeElapsed);
}

void Player::OpenSpawnWindow() {
	AddCommandToClient(new network::protocol::server::OpenSpawnWindowCommand());
}

void Player::Suspend() {
	camera->Suspend();
}

void Player::SetControl(Control *control) {
	if (this->control) this->control->player = nullptr;
    this->control = control;
	control->player = this;
	control->GetUI()->NeedFullRefresh();
    SetCamera(new Camera(control->GetOwner()->GetTile()));
    camera->SetPlayer(this);
	camera->TrackObject(control->GetOwner());
	camera->SetInvisibleVisibility(control->GetSeeInvisibleAbility());
};

void Player::SetCamera(Camera *camera) { this->camera.reset(camera); }

sptr<Connection> Player::GetConnection() {
	return connection.lock();
}

Control *Player::GetControl() { return control; }
Camera *Player::GetCamera() { return camera.get(); }
bool Player::IsConnected() { return !connection.expired(); }

void Player::AddCommandToClient(network::protocol::Command *command) {
	if (sptr<Connection> connect = connection.lock())
		connect->commandsToClient.Push(command);
}

void Player::AddSyncCommandFromClient(uptr<network::protocol::Command> &&command) {
	syncCommands.Push(std::forward<uptr<network::protocol::Command>>(command));
}
