#ifndef DIAGNOSTIC_HPP_
#define DIAGNOSTIC_HPP_

#include <iostream>
#include <iomanip>
#include <sstream>
#include <optional>
#include <vector>
#include <string>
#include <expected>

#include "str.hpp"

enum class Level {
    Critical, Warning, Help
};

/**
 * @brief Region describes a span of the source code, where an error occured.
 */
struct Region {
    std::string file_name;
    std::pair<int, int> location; // coordinates of the location where the error happend.
    std::vector<std::string> source_code;

    Region(
        std::string& f,
        int line,
        int col,
        std::vector<std::string> sc
    ) : file_name(f),
        location(std::make_pair(line, col)),
        source_code(std::move(sc)) {}
};

/**
 * @brief Labeles represent the error of a diagnostic in a better context.
 * This is done by explaining the user where the error emerged from, pointed by the label symbol and text.
 *   ____________________________________________________
 *  |                                                    |
 *  |                                                    |
 * [l]  let str: String = "name;                         |
 *  |                     ^ unterminated literal         |
 *  |                       --------------------         |
 *  |                              Label                 |
 *  |____________________________________________________|
 * 
*/
struct Label {
    std::string text;
    std::vector<Region> regions;

    Label(std::string t, std::vector<Region> r) 
        : text(t), regions(std::move(r)) {}

    void render_label();
};

struct Diagnostic {
    Level level;
    std::string message;
    std::string hint;
    std::vector<Label> labels;

    Diagnostic(
        Level l, 
        std::string msg, 
        std::string h,
        std::vector<Label> label_vec
    ) : level(l),
        message(msg),
        hint(h),
        labels(std::move(label_vec)) {}

    void pretty_print() const;
    constexpr auto level_to_str() const -> std::string;
};

struct Diagnostics {
    std::vector<Diagnostic> diags;

    Diagnostics(int max_cap) : diags() {
        diags.reserve(max_cap);
    };
};

constexpr std::string RED_BOLD     {"\033[1;31m"};
constexpr std::string GREEN_BOLD   {"\033[1;32m"};
constexpr std::string YELLOW_BOLD  {"\033[1;33m"};
constexpr std::string BLUE_BOLD    {"\033[1;34m"};
constexpr std::string CYAN_BOLD    {"\033[1;36m"};
constexpr std::string RESET        {"\033[0m"};

/**
 * @brief Renderer defines the textual format of the Wombat diagnostic system.
 *
 * Structure:
 *                                    _
 *          [Level]: [Message]         | Header
 *      _   At [File:Line:Column]     _|
 *     |   |                                                      _
 *     |   |                                                       |
 *     |  [*] [Relevant source-code]                               | Marker
 * Arm |   |   ^^^^^^ ---- [Labels pointing to relevant parts]     |
 *     |   .                                                      _|
 *     |   .
 *     |   .
 *     |_  |                                                               
 *         ~ [Hints, suggestions, or additional information]     <-- Hint
 * 
 */
struct Renderer {

    //! Header will represent a textual-string that summarises the diagnostic.
    struct Header {
        std::string level;
        std::string message;

        Header(std::string l, std::string m)
            : level(l), message(m) {};

        auto format() -> std::ostringstream {
            std::ostringstream formatted_header;

            auto col = (level == "critical") ? RED_BOLD : YELLOW_BOLD;

            formatted_header << col << level << RESET << ": " << message;
            return formatted_header;
        };
    };

    struct Marker {
        int origin;
        Region reg;
        std::string region_msg;

        Marker(int o, Region r, std::string m)
            : origin(o), reg(std::move(r)), region_msg(m) {}

        std::ostringstream format() const {
            std::ostringstream out;

            out << "at " << reg.file_name << ":" 
                << reg.location.first + 1 << ":"
                << reg.location.second << "\n";

            const auto& [trimmed_line, offset] = left_trim(reg.source_code.front());

            out << " |\n"
                << "[" << CYAN_BOLD << reg.location.first + 1 << RESET << "] " << trimmed_line << "\n" 
                << " |";

            int caret_position = reg.location.second + 1 - offset;
            if (caret_position < 0) caret_position = 0;
        
            out << std::string(caret_position, ' ') << "^ " << region_msg;
        
            for (size_t k = 1; k < reg.source_code.size(); ++k) {
                out << " |" << reg.source_code[k] << "\n";
            }

            return out;
        }       
    };

    Renderer() = default;
    
    void render_short(const Diagnostic& diag) const;
    void render_pretty_print(const Diagnostic& diag) const;
};

#endif // DIAGNOSTIC_HPP_