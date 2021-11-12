#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "nlohmann/json.hpp"
#include "digital_curling/digital_curling.hpp"

using namespace digital_curling;
using namespace digital_curling::game;
using namespace digital_curling::game::moves;
using namespace digital_curling::simulation;

namespace {

/// <summary>
/// 試合の識別に使うID
/// </summary>
std::string game_id;

/// <summary>
/// 試合設定
/// </summary>
Setting game_setting;

/// <summary>
/// ストーンの物理シミュレーションを行うシミュレータの設定
/// </summary>
std::unique_ptr<ISimulatorSetting> simulator_setting;

/// <summary>
/// ストーンの物理シミュレーションを行うシミュレータ
/// </summary>
std::unique_ptr<ISimulator> simulator;

/// <summary>
/// このクライアントのチームID．最初のエンドの先攻は<see cref="digital_curling::game::Team::k0"/>．
/// </summary>
Team team;

/// <summary>
/// (エクストラでない)エンドの時間制限
/// </summary>
std::chrono::seconds time_limit;

/// <summary>
/// エクストラエンドの時間制限
/// </summary>
std::chrono::seconds extra_time_limit;

/// <summary>
/// 現在の試合の状態
/// </summary>
State game_state;

/// <summary>
/// 現在の残り時間．
/// </summary>
std::array<std::chrono::seconds, 2> remaining_times;

/// <summary>
/// 前回の行動．<see cref="OnMyTurn"/>内で参照した場合は相手チームの行動になり，
/// <see cref="OnOpponentTurn"/>内で参照した場合は自チームの行動になる．
/// </summary>
std::optional<Move> last_move;

/// <summary>
/// 前回のエンドの最終ストーン位置
/// </summary>
std::array<std::optional<Vector2>, kStoneMax> last_end_stone_positions;



/// <summary>
/// AIにとって準備に時間がかかる処理を行う．
/// </summary>
void OnInit()
{
    // TODO AIを作る際はここを編集してください
}



/// <summary>
/// 自チームのターンに呼ばれます．行動を選択し，返します．
/// </summary>
/// <returns>選択された行動</returns>
Move OnMyTurn()
{
    Move move;

    // TODO AIを作る際はここを編集してください(下記のwhile文は消してください)

    // このサンプルでは標準入力を受け取ってMoveに変換しています．
    while (true) {
        std::cout << "input stone velocity (ex1: 0.1 2.5 ccw) (ex2: concede): ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            throw std::runtime_error("std::getline");
        }
        if (line == "concede") {
            move = Concede();
            break;
        }
        Shot shot;
        std::string shot_rotation_str;
        std::istringstream line_buf(line);
        line_buf >> shot.velocity.x >> shot.velocity.y >> shot_rotation_str;

        if (shot_rotation_str == "cw") {
            shot.rotation = Shot::Rotation::kCW;
        } else if (shot_rotation_str == "ccw") {
            shot.rotation = Shot::Rotation::kCCW;
        } else {
            continue;
        }

        move = shot;
        break;
    }

    return move;
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

    constexpr auto kName = "ex1-stdio";

    try {
        if (argc != 3) {
            std::cerr << "Usage: command <host> <port>" << std::endl;
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::socket socket(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(socket, resolver.resolve(argv[1], argv[2]));

        std::string input_buffer;

        // [in] dc
        {
            input_buffer.clear();
            boost::asio::read_until(socket, boost::asio::dynamic_buffer(input_buffer), '\n');
            std::cout << "[in] " << input_buffer << std::flush;

            auto const jin = json::parse(input_buffer);
            if (jin.at("cmd").get<std::string>() != "dc") {
                throw std::runtime_error("Unexpected cmd");
            }
            if (jin.at("version") != 1) {
                throw std::runtime_error("Version error");
            }
        }

        // [out] dc_ok
        {
            json const jout = {
                { "cmd", "dc_ok" },
                { "version", 1 }
            };
            auto const output_message = jout.dump() + '\n';
            std::cout << "[out] " << output_message << std::flush;
            boost::asio::write(socket, boost::asio::buffer(output_message));
        }


        // [in] is_ready
        {
            input_buffer.clear();
            boost::asio::read_until(socket, boost::asio::dynamic_buffer(input_buffer), '\n');
            std::cout << "[in] " << input_buffer << std::flush;

            auto const jin = json::parse(input_buffer);

            if (jin.at("cmd").get<std::string>() != "is_ready") {
                throw std::runtime_error("Unexpected cmd");
            }

            if (jin.at("rule").get<std::string>() != "normal") {
                throw std::runtime_error("Unexpected rule");
            }

            game_id = jin.at("game_id").get<std::string>();

            game_setting = jin.at("game_setting").get<::digital_curling::game::Setting>();

            simulator_setting = jin.at("simulator_setting").get<std::unique_ptr<::digital_curling::simulation::ISimulatorSetting>>();

            simulator = simulator_setting->CreateSimulator();

            team = jin.at("team").get<::digital_curling::game::Team>();

            time_limit = std::chrono::seconds(jin.at("time_limit").get<std::chrono::seconds::rep>());
            extra_time_limit = std::chrono::seconds(jin.at("extra_time_limit").get<std::chrono::seconds::rep>());
        }

        OnInit();

        // [out] ready_ok
        {
            json const jout = {
                { "cmd", "ready_ok" },
                { "name", kName }
            };
            auto const output_message = jout.dump() + '\n';
            std::cout << "[out] " << output_message << std::flush;
            boost::asio::write(socket, boost::asio::buffer(output_message));
        }

        // [in] new_game
        {
            input_buffer.clear();
            boost::asio::read_until(socket, boost::asio::dynamic_buffer(input_buffer), '\n');
            std::cout << "[in] " << input_buffer << std::flush;

            auto const jin = json::parse(input_buffer);
            if (jin.at("cmd").get<std::string>() != "new_game") {
                throw std::runtime_error("Unexpected cmd");
            }
        }

        while (true) {
            // [in] update
            input_buffer.clear();
            boost::asio::read_until(socket, boost::asio::dynamic_buffer(input_buffer), '\n');
            std::cout << "[in] " << input_buffer << std::flush;

            auto const jin = json::parse(input_buffer);
            if (jin.at("cmd").get<std::string>() != "update") {
                throw std::runtime_error("Unexpected cmd");
            }

            game_state = jin.at("state").get<::digital_curling::game::State>();
            for (size_t i = 0; i < 2; ++i) {
                remaining_times[i] = std::chrono::seconds(jin.at("remaining_times").at(i).get<std::chrono::seconds::rep>());
            }
            last_move = jin.at("last_move").get<std::optional<::digital_curling::game::Move>>();
            if (auto const & jin_last_end_stone_positions = jin.at("last_end_stone_positions"); !jin_last_end_stone_positions.is_null()) {
                jin_last_end_stone_positions.get_to(last_end_stone_positions);
            }

            // if game was over
            if (game_state.result) {
                break;
            }

            if (game_state.GetCurrentTeam() == team) { // my turn
                // [out] move
                auto move = OnMyTurn();
                assert(!std::holds_alternative<::digital_curling::game::moves::TimeLimit>(move));  // move type TimeLimit は使用しないで下さい．
                json jout = move;
                jout["cmd"] = "move";

                auto const output_message = jout.dump() + '\n';
                std::cout << "[out] " << output_message << std::flush;
                boost::asio::write(socket, boost::asio::buffer(output_message));
            } else { // opponent turn
                OnOpponentTurn();
            }
        }

        // [in] game_over
        {
            input_buffer.clear();
            boost::asio::read_until(socket, boost::asio::dynamic_buffer(input_buffer), '\n');
            std::cout << "[in] " << input_buffer << std::flush;

            auto const jin = json::parse(input_buffer);
            if (jin.at("cmd").get<std::string>() != "game_over") {
                throw std::runtime_error("Unexpected cmd");
            }
        }

        // 終了．
        OnGameOver();

    } catch (std::exception & e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
