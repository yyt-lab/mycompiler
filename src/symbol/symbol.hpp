/*****************************************************
 *  Definition of Symbol's.
 *
 *  This file contains the definition of three kind of symbols:
 *    1. Function - representing a function definition
 *    2. Variable - representing a variable
 *
 *  Keltin Leung 
 */

#pragma once

// #include "define.hpp"
#include "scope.hpp"
#include "type.hpp"
#include "tac.hpp"
#include <iostream>
#include <string>

// namespace mind {

// #define MIND_SYMB_DEFINED
// namespace symb {
class Scope;
/* Representation of the data objects in Mind.
 */
class Symbol {
  protected:
    // name of this symbol
    std::string name;
    // type of this symbol
    Type *type;
    // definition location in the source code
    Location *loc;
    // the scope where this symbol is defined
    Scope *defined_in;
    // the order of this symbol
    int order;

  public:
    // auxiliary mark
    int mark;
    // offset of this symbol
    int offset;
    // Gets the name of this symbol
    virtual std::string getName(void);
    // Gets the type of this symbol
    virtual Type *getType(void);
    // Gets the definition location
    virtual Location *getDefLocation(void);
    // Gets the order
    virtual int getOrder(void);
    // Sets the order
    virtual void setOrder(int order);
    // Tests whether it is a Variable
    virtual bool isVariable(void);
    // Tests whether it is a Function
    virtual bool isFunction(void);
    // Gets the scope where this symbol was declared
    virtual Scope *getScope(void);
    // Sets the scope of this symbol
    virtual void setScope(Scope *);
    // Prints this symbol
    virtual void dump(std::ostream &) = 0;

    virtual ~Symbol() {}
};

/* Variable Declaration.
 */
class Variable : public Symbol {
  private:
    // the parameter flag
    bool is_parameter;
    // the attached register object
    Temp attached;
    // init value for global variable
    int global_init;

  public:
    // Constructor
    Variable(std::string n, Type *t, Location *l);
    // Sets the parameter flag
    void setParameter(void);
    // Tests whether it is a parameter
    bool isParameter(void);
    // Set init value for a global variable
    void setGlobalInit(int val);
    // Get the init value for a global variable
    int getGlobalInit();
    // Tests whether it is a global variable
    bool isGlobalVar(void);
    // Tests whether it is a local variable
    bool isLocalVar(void);
    // Tests whether this symbol is a Variable
    virtual bool isVariable(void);
    // Prints this symbol
    virtual void dump(std::ostream &os);
    // Attaches a temporary variable object to this symbol
    void attachTemp(Temp);
    // Gets the attached register object
    Temp getTemp(void);
};

/* Function Definition.
 */
class Function : public Symbol {
  private:
    // the associated scope
    // scope::FuncScope *associated;
    // the entry label
    Label entry;
    // the associated Functy object
    Functy attached;

  public:
    // Constructor
    Function(std::string n, Type *resType, Location *l);
    // Gets the associated scope
    // scope::FuncScope *getAssociatedScope(void);
    // Gets the result type
    Type *getResultType(void);
    // Gets the scope where this symbol was declared
    // virtual scope::Scope *getScope(void);
    // Gets the type of this symbol
    virtual FuncType *getType(void);
    // Appends a parameter to this function
    void appendParameter(Variable *);
    // Tests whether this symbol is a Function
    virtual bool isFunction(void);
    // Prints this symbol
    virtual void dump(std::ostream &os);

  public:
    // Attaches a Functy object to this symbol
    void attachFuncty(Functy);
    // Gets the attached Functy object
    Functy getFuncty(void);
    // Attaches the entry label to this function
    void attachEntryLabel(Label);
    // Gets the entry label of this function
    Label getEntryLabel(void);
};

// } // namespace symb
// } // namespace mind

