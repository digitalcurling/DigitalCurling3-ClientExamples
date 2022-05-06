#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "digital_curling/digital_curling.hpp"

namespace dc = digital_curling;

namespace {


/// \brief 試合ID
std::string g_game_id;

std::string g_date_time;

/// \brief このクライアントのチームID
///
/// 最初のエンドの先攻は \c Team::k0
dc::Team g_team;

/// \brief 試合設定
dc::GameSetting g_game_setting;

/// \brief 自チームのプレイヤー設定
std::vector<std::unique_ptr<dc::IPlayerFactory>> g_player_factories;

/// \brief 自チームのプレイヤー
std::vector<std::unique_ptr<dc::IPlayer>> g_players;

/// \brief ストーンの物理シミュレータの設定
std::unique_ptr<dc::ISimulatorFactory> g_simulator_factory;

/// \brief ストーンの物理シミュレータ
std::unique_ptr<dc::ISimulator> g_simulator;

/// \brief 現在の試合の状態
dc::GameState g_game_state;

/// \brief AIにとって準備に時間がかかる処理を行う．
///
/// \param player_order プレイヤーの順番(デフォルトで0, 1, 2, 3)．プレイヤーの順番を変更したい場合は変更してください．
void OnInit(std::vector<int> & player_order)
{
    // TODO AIを作る際はここを編集してください
}

/// <summary>
/// 自チームのターンに呼ばれます．行動を選択し，返します．
/// </summary>
/// <returns>選択された行動</returns>
dc::Move OnMyTurn()
{
    dc::moves::Shot shot;

    // TODO AIを作る際はこのあたりを編集してください

    // ショットの初速
    shot.velocity.x = 0.132f;
    shot.velocity.y = 2.3995f;

    // ショットの回転
    shot.rotation = dc::moves::Shot::Rotation::kCCW; // 反時計回り
    // shot.rotation = dc::moves::Shot::Rotation::kCW; // 時計回り

    return shot;

    // コンシードを行う場合
    // return dc::moves::Concede();
}



/// <summary>
/// 相手チームのターンに呼ばれます．AIを作る際にこの関数の中身を記述する必要は無いかもしれません．
/// </summary>
void OnOpponentTurn()
{
    // TODO AIを作る際はここを編集してください
}



/// <summary>
/// ゲームが正常に終了した際にはこの関数が呼ばれます．
/// </summary>
void OnGameOver()
{
    // TODO AIを作る際はここを編集してください
}



} // unnamed namespace



int main(int argc, char const * argv[])
{
    using boost::asio::ip::tcp;
    using nlohmann::json;

    // TODO 標準出力にログを出したくない場合は false にしてください
    constexpr bool kOutputLog = true;

    // TODO AIの名前を変更する場合はここを変更してください．
    constexpr auto kName = "ex0-template";

    try {
        if (argc != 3) {
            std::cerr << "Usage: command <host> <port>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(socket, resolver.resolve(argv[1], argv[2]));

        auto read_next_line = [&socket, input_buffer = std::string()] () mutable {
            // read_untilの結果，input_bufferに複数行入ることがあるため，1行ずつ取り出す処理を行っている
            if (input_buffer.empty()) {
                boost::asio::read_until(socket, boost::asio::dynamic_buffer(input_buffer), '\n');
            }
            auto new_line_pos = input_buffer.find_first_of('\n');
            auto line = input_buffer.substr(0, new_line_pos + 1);
            input_buffer.erase(0, new_line_pos + 1);
            return line;
        };

        // [in] dc
        {
            auto const line = read_next_line();
            auto const jin = json::parse(line);

            if (jin.at("cmd").get<std::string>() != "dc") {
                throw std::runtime_error("Unexpected cmd");
            }

            if (jin.at("version").at("major").get<int>() != 2) {
                throw std::runtime_error("Unexpected version");
            }

            g_game_id = jin.at("game_id").get<std::string>();
            g_date_time = jin.at("date_time").get<std::string>();
            
            if constexpr (kOutputLog) {
                std::cout << "[in] dc" << std::endl;
            }
        }

        // [out] dc_ok
        {
            json const jout = {
                { "cmd", "dc_ok" },
                { "name", kName }
            };
            auto const output_message = jout.dump() + '\n';
            boost::asio::write(socket, boost::asio::buffer(output_message));

            if constexpr (kOutputLog) {
                std::cout << "[out] dc_ok" << std::endl;
            }
        }


        // [in] is_ready
        {
            auto const line = read_next_line();
            auto const jin = json::parse(line);

            if (jin.at("cmd").get<std::string>() != "is_ready") {
                throw std::runtime_error("Unexpected cmd");
            }

            if (jin.at("game").at("rule").get<std::string>() != "normal") {
                throw std::runtime_error("Unexpected rule");
            }

            g_team = jin.at("team").get<dc::Team>();

            g_game_setting = jin.at("game").at("setting").get<dc::GameSetting>();

            g_player_factories.clear();
            for (auto const& jin_player_factory : jin.at("game").at("players").at(dc::ToString(g_team))) {
                g_player_factories.emplace_back(jin_player_factory.get<std::unique_ptr<dc::IPlayerFactory>>());
                g_players.emplace_back(g_player_factories.back()->CreatePlayer());
            }

            g_simulator_factory = jin.at("game").at("simulator").get<std::unique_ptr<dc::ISimulatorFactory>>();
            g_simulator = g_simulator_factory->CreateSimulator();

            if constexpr (kOutputLog) {
                std::cout << "[in] is_ready" << std::endl;
            }
        }
        
        // [out] ready_ok
        {
            std::vector<int> player_order{ 0, 1, 2, 3 };
            OnInit(player_order);

            json const jout = {
                { "cmd", "ready_ok" },
                { "player_order", player_order }
            };
            auto const output_message = jout.dump() + '\n';
            boost::asio::write(socket, boost::asio::buffer(output_message));

            if constexpr (kOutputLog) {
                std::cout << "[out] ready_ok" << std::endl;
            }
        }

        // [in] new_game
        {
            auto const line = read_next_line();
            auto const jin = json::parse(line);

            if (jin.at("cmd").get<std::string>() != "new_game") {
                throw std::runtime_error("Unexpected cmd");
            }

            if constexpr (kOutputLog) {
                std::cout << "[in] new_game (" << jin.at("name").at("team0") << " vs " << jin.at("name").at("team1") << std::endl;
            }
        }

        while (true) {
            // [in] update
            auto const line = read_next_line();
            auto const jin = json::parse(line);

            if (jin.at("cmd").get<std::string>() != "update") {
                throw std::runtime_error("Unexpected cmd");
            }

            g_game_state = jin.at("state").get<dc::GameState>();

            if constexpr (kOutputLog) {
                std::cout << "[in] update (end: " << int(g_game_state.end) << ", shot: " << int(g_game_state.shot) << ")" << std::endl;
            }

            // if game was over
            if (g_game_state.game_result) {
                break;
            }

            if (g_game_state.GetNextTeam() == g_team) { // my turn
                // [out] move
                auto move = OnMyTurn();
                json jout = {
                    { "cmd", "move" },
                    { "move", move }
                };
                auto const output_message = jout.dump() + '\n';
                boost::asio::write(socket, boost::asio::buffer(output_message));
                
                if constexpr (kOutputLog) {
                    std::cout << "[out] move" << std::endl;
                }
            } else { // opponent turn
                OnOpponentTurn();
            }
        }

        // [in] game_over
        {
            auto const line = read_next_line();
            auto const jin = json::parse(line);

            if (jin.at("cmd").get<std::string>() != "game_over") {
                throw std::runtime_error("Unexpected cmd");
            }

            if constexpr (kOutputLog) {
                std::cout << "[in] game_over" << std::endl;
            }
        }

        // 終了．
        OnGameOver();

    } catch (std::exception & e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
    }

    return 0;
}
