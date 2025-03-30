#ifndef READABLES_HPP_
#define READABLES_HPP_

/**
 * @brief ReadableType defines the allowed readables in wombat.
 * 
 *  Readables!, wombat offers a core-syntax language feature! 
 *  See [readables] in wombat-docs.
 *  Briefly, readables make developers code cleaner and readable code!
 *  They operate on a single-argument and they are notated by the '!' symbol.s
 */
enum class ReadableType {
    // extract! -> computes an entire block expressions.
    // `
    // let first: String = "wombat";
    // let second: String = extract! {              
    //      if name == "wombat" {
    //          return language
    //      }
    //      return "..?"       
    //  };
    // `
    Extract,

    // address! -> returns the memory location of a given identifier.
    // See [token.hpp] or the above for detailed example.
    Address,

    // Castors are readables designed for casting. 
    // castor! { value_to_cast };
    // e.g.
    // ` 
    // let floored: int = int_castor! { 3.14 }; 
    // write("value: {}", floored); // prints out '3'
    // `
    IntCastor,
    FloatCastor,
    CharCastor,
    StringCastor
};



#endif // READABLES_HPP_