#ifndef _NODE_H_
#define _NODE_H_

#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

class CodeGenContext;
class NStatement;
class NExpression;
class NIdentifier;
class NVariableDecl;
class NParamDecl;
class NStructDecl;
class NUnionDecl;
class NSpecifier;
class NFunctionDecl;

template <typename T1, typename T2, typename T3>
class InitDeclarator
{
public:
	T1 first;
	T2 second;
	T3 third;

	InitDeclarator(T1 first, T2 second, T3 third) :
	first(first), second(second), third(third) { }
};

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDecl*> VariableList;
typedef std::vector<NParamDecl*> ParamList;
typedef std::vector<NExpression*> ArrayDim;
typedef std::vector<NSpecifier*> DeclSpecifier;

typedef InitDeclarator<std::string, ArrayDim*, NExpression*> Declarator;
typedef std::vector<Declarator *> DeclaratorList;

// Type Specifier
class NType {
public:
	int line_number = -1;
	virtual llvm::Type* getType(CodeGenContext& context);
};

class NIdentifierType : public NType {
public:
	int line_number = -1;
	NIdentifier& type;
	
	NIdentifierType(NIdentifier& type) :
	type(type) { }

	virtual llvm::Type* getType(CodeGenContext& context);
};

class NTypeof : public NType {
public:
	int line_number = -1;
	NExpression& operand;
	
	NTypeof(NExpression& operand) :
	operand(operand) { }

	virtual llvm::Type* getType(CodeGenContext& context);
};

class NDerivedType : public NType {
public:
	int line_number = -1;
	NType& base;
	int ptrDim;
	int arrDim;

	NDerivedType(NType& base, int ptrDim, int arrDim) :
	base(base), ptrDim(ptrDim), arrDim(arrDim) { }

	virtual llvm::Type* getType(CodeGenContext& context);
};

class NStructType : public NType {
public:
	int line_number = -1;
	NStructDecl *struct_decl;
	const NIdentifier *id;

    NStructType(NStructDecl *struct_decl) :
	id(NULL), struct_decl(struct_decl) { }

    NStructType(const NIdentifier *id) :
	id(id), struct_decl(NULL) { }

    virtual llvm::Type* getType(CodeGenContext& context);
};

class NUnionType : public NType {
public:
	int line_number = -1;
	NUnionDecl *union_decl;
	const NIdentifier *id;

    NUnionType(NUnionDecl *union_decl) :
	id(NULL), union_decl(union_decl) { }

    NUnionType(const NIdentifier *id) :
	id(id), union_decl(NULL) { }

    virtual llvm::Type* getType(CodeGenContext& context);
};

class NBitFieldType : public NType {
public:
	int line_number = -1;
	unsigned N;

    NBitFieldType(unsigned N) :
	N(N) { }

    virtual llvm::Type* getType(CodeGenContext& context);
};

// Node
class Node {
public:
	int line_number = -1;
	virtual ~Node() {}
	virtual llvm::Value* codeGen(CodeGenContext& context) { return NULL; }
};
class NStatement : public Node {
public:
	int line_number = -1;
};
class NExpression : public NStatement {
public:
	int line_number = -1;
};

// Primary Expression
// Constant
class NVoid : public NExpression {
	int line_number = -1;
};
class NInteger : public NExpression {
public:
	int line_number = -1;
	const char *value;

	NInteger(const char *value) :
	value(value) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};
class NBoolean : public NExpression {
public:
	int line_number = -1;
	bool value;

	NBoolean(bool value) :
	value(value) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};
class NDouble : public NExpression {
public:
	int line_number = -1;
	double value;

	NDouble(double value) :
	value(value) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};
class NString : public NExpression {
public:
	int line_number = -1;
	std::string value;

	NString(const std::string& value) :
	value(value) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};
class NIdentifier : public NExpression {
public:
	int line_number = -1;
	std::string name;

	NIdentifier(const std::string& name) :
	name(name) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};
// Primary Expression End

// Postfix Expression
class NMethodCall : public NExpression {
public:
	int line_number = -1;
	NExpression& func_expr;
	ExpressionList arguments;

	NMethodCall(NExpression& func_expr, ExpressionList& arguments) :
	func_expr(func_expr), arguments(arguments) { }

	NMethodCall(NExpression& func_expr) :
	func_expr(func_expr) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};
class NFieldExpr : public NExpression {
public:
	int line_number = -1;
	NExpression& operand;
	NIdentifier& field_name;

	NFieldExpr(NExpression& operand, NIdentifier& field_name) :
	operand(operand), field_name(field_name) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};
class NArrayExpr : public NExpression {
public:
	int line_number = -1;
	NExpression& operand;
	NExpression& index;

	NArrayExpr(NExpression& operand, NExpression& index) :
	operand(operand), index(index) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};
// Postfix Expression End

// Binary Expression
class NBinaryExpr : public NExpression {
public:
	int line_number = -1;
	int op;
	NExpression& lval;
	NExpression& rval;

	NBinaryExpr(NExpression& lval, int op, NExpression& rval) :
	lval(lval), rval(rval), op(op) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

// Prefix Expression
class NPrefixExpr : public NExpression {
public:
	int line_number = -1;
	int op;
	NType& type;
	NExpression& operand;

	NPrefixExpr(int op, NExpression& operand) :
	op(op), operand(operand),
	type(*new NType()) { }

	NPrefixExpr(NType& type, NExpression& operand) :
	type(type), operand(operand), op(-1) { }

	NPrefixExpr(int op, NType& type) :
	type(type), operand(*new NExpression()), op(op) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignmentExpr : public NExpression {
public:
	int line_number = -1;
	NExpression& lval;
	NExpression& rval;

	NAssignmentExpr(NExpression& lval, NExpression& rval) :
	lval(lval), rval(rval) { }

	static llvm::Value *doAssignCast(CodeGenContext& context, llvm::Value *value,
									  llvm::Type *variable_type, llvm::Value *variable, int line_number);

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBlock : public NStatement {
public:
	int line_number = -1;
	StatementList statements;

	NBlock() { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NReturnStatement : public NStatement {
public:
	int line_number = -1;
	NExpression& expression;

	NReturnStatement(NExpression& expression) :
	expression(expression) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIfStatement : public NStatement {
public:
	int line_number = -1;
	NExpression& condition;
	NStatement *if_true;
	NStatement *if_else;

	NIfStatement(NExpression& condition, NStatement *if_true, NStatement *if_else) :
	condition(condition), if_true(if_true), if_else(if_else) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NParamDecl : public NStatement {
public:
	int line_number = -1;
	NType& type;
	NIdentifier& id;
	ArrayDim& array_dim;
	NExpression *initializer = NULL;

	NParamDecl(NType& type, NIdentifier& id, ArrayDim& array_dim) :
	type(type), id(id), array_dim(array_dim) { }

	NParamDecl(NType& type, NIdentifier& id, ArrayDim& array_dim, NExpression *initializer) :
	type(type), id(id), array_dim(array_dim), initializer(initializer) { }

	// virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NDelegateDecl : public NStatement {
public:
	int line_number = -1;
	NType& type;
	const NIdentifier& id;
	bool has_vargs;
	ParamList arguments;

	NDelegateDecl(NType& type, const NIdentifier& id,
				  const ParamList& arguments, bool has_vargs) :
	type(type), id(id), arguments(arguments), has_vargs(has_vargs) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NStructDecl : public NStatement {
public:
	int line_number = -1;
	NIdentifier& id;
    const VariableList& fields;

    NStructDecl(NIdentifier& id, const VariableList& fields) :
	id(id), fields(fields) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NUnionDecl : public NStatement {
public:
	int line_number = -1;
	NIdentifier& id;
    const VariableList& fields;

    NUnionDecl(NIdentifier& id, const VariableList& fields) :
	id(id), fields(fields) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NTypedefDecl : public NStatement {
public:
	int line_number = -1;
	NType& type;
	NIdentifier& id;
	ArrayDim array_dim;

    NTypedefDecl(NType& type, NIdentifier& id, ArrayDim array_dim) :
	type(type), id(id), array_dim(array_dim) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class SpecifierSet {
public:
	bool is_static = false;
	NType *type = NULL;
};

class NSpecifier {
public:
	virtual void setSpecifier(SpecifierSet *dest);
};

class NExternSpecifier : public NSpecifier {
public:
	virtual void setSpecifier(SpecifierSet *dest);
};

class NStaticSpecifier : public NSpecifier {
public:
	virtual void setSpecifier(SpecifierSet *dest);
};

class NTypeSpecifier : public NSpecifier {
public:
	NType& type;

	NTypeSpecifier(NType& type) :
	type(type) {}

	virtual void setSpecifier(SpecifierSet *dest);
};

class NVariableDecl : public NStatement {
public:
	int line_number = -1;
	DeclSpecifier& var_specifier;
	DeclaratorList *declarator_list;

	SpecifierSet *specifiers;

	NVariableDecl(DeclSpecifier& var_specifier, DeclaratorList *declarator_list) :
	var_specifier(var_specifier), declarator_list(declarator_list) { specifiers = new SpecifierSet(); }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDecl : public NStatement {
public:
	int line_number = -1;
	DeclSpecifier& func_specifier;
	const NIdentifier& id;
	bool has_vargs;
	ParamList arguments;
	NBlock *block;

	// specifiers: will be set by specifier
	SpecifierSet *specifiers;

	NFunctionDecl(DeclSpecifier& func_specifier, const NIdentifier& id,
				  const ParamList& arguments, NBlock *block, bool has_vargs) :
	func_specifier(func_specifier), id(id), arguments(arguments), block(block),
	has_vargs(has_vargs) { specifiers = new SpecifierSet();  }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

#endif
