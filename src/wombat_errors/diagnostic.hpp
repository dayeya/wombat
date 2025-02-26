#ifndef DIAGNOSTIC_HPP_
#define DIAGNOSTIC_HPP_

#include <iostream>
#include <iomanip>
#include <sstream>
#include <optional>
#include <vector>
#include <string>
#include <expected>

enum class Phase {
    Precomp,
    Lexer,
    Parser,
    CodeGen,
    Optimize,
    SessInit
};

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
 *  |                         ^ unterminated literal     |
 *  |                           --------------------     |
 *  |                                  Label             |
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
    Phase phase;
    std::string message;
    std::string hint;
    std::vector<Label> labels;

    Diagnostic(
        Level l, 
        Phase p, 
        std::string msg, 
        std::string h,
        std::vector<Label> label_vec
    ) : level(l),
        phase(p),
        message(msg),
        hint(h),
        labels(std::move(label_vec)) {}

    void pretty_print() const;
    constexpr auto level_to_str() const -> std::string;
    constexpr auto phase_to_str() const -> std::string;
};

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
            std::ostringstream sstream;
            sstream << level << ": " << message << ".";
            return sstream;
        };
    };

    //! Marker will represent a textual-string that contains the label of the source-code.
    struct Marker {
        int origin;
        Region reg;
        std::string region_msg;


        Marker(int o, Region r, std::string m)
            : origin(o), reg(std::move(r)), region_msg(m) {}

        auto format() -> std::ostringstream {
            std::ostringstream sstream;
            sstream << " |" << "\n";
            sstream << "[" 
                    << reg.location.first + 1 
                    << "] ";
                    
            for(int k = 0; k < reg.source_code.size(); ++k) {
                if(k == 0) {
                    sstream << reg.source_code.at(k) << "\n";
                    sstream << " |" 
                            << std::string(reg.location.second + 1, ' ') 
                            << "^ " 
                            << region_msg; 
                } else {
                    sstream << " |" 
                            << reg.source_code[k] ;
                }
            }
            return sstream;
        }
    };

    Renderer() {};
    
    void render_short(const Diagnostic& diag) const;
    void render_pretty_print(const Diagnostic& diag) const;
};

#endif // DIAGNOSTIC_HPP_