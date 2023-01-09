/* Implementation of Recursive-Descent Parser
 * parseInt.cpp
 * Programming Assignment 3
 * Fall 2022
*/

#include "parseInt.h"

map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects
bool cond = true;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);


//Program is: Prog ::= PROGRAM IDENT StmtList END PROGRAM
bool Prog(istream& in, int& line)
{
	bool f1;
	LexItem tok = Parser::GetNextToken(in, line);
		
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			
			f1 = StmtList(in, line); 
			
			if(f1) {
				tok = Parser::GetNextToken(in, line);
				if( tok == END){
					tok = Parser::GetNextToken(in, line);
					if(tok != PROGRAM){
						ParseError(line, "Missing PROGRAM Keyword.");
						return false;
					}
					return true;
				}
				else
				{
					ParseError(line, "Missing END of Program.");
					return false;
				}
				
			}
			else
			{
				ParseError(line, "Incorrect Program Body.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program Name.");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return true;
	}
	ParseError(line, "Missing PROGRAM.");
	return false;
}//End of Prog

//StmtList ::= Stmt; { Stmt; }
bool StmtList(istream& in, int& line){
	bool status;
		
	LexItem tok;
	
	status = Stmt(in, line);
		
	while(status)
	{
		tok = Parser::GetNextToken(in, line);
		if(tok != SEMICOL)
		{
			ParseError(line, "Missing semicolon at end of Statement.");
			return false;
		}
			
		status = Stmt(in, line);
	}
			
	tok = Parser::GetNextToken(in, line);
	if(tok == END )
	{
		Parser::PushBackToken(tok);
		return true;
	}
	else if(tok == ELSE)
	{
		Parser::PushBackToken(tok);
		return true;
	}
	else 
	{
		ParseError(line, "Syntactic error in Program Body.");
		return false;
	}
	
}//End of StmtList function

//Stmt ::= DeclStmt | ControlStmt
bool Stmt(istream& in, int& line){
	bool status=false;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {
	case INT: case FLOAT: case BOOL:
		Parser::PushBackToken(t);
		status = DeclStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect Declaration Statement.");
			return status;
		}
		break;
	case IF: case PRINT: case IDENT: 
		Parser::PushBackToken(t);
		status = ControlStmt(in, line);
		
		if(!status)
		{
			ParseError(line, "Incorrect control Statement.");
			return status;
		}
		break;
	default:
		Parser::PushBackToken(t);
	}
	return status;
}//End of Stmt function

//DeclStmt ::= ( INT | FLOAT | BOOL ) VarList
bool DeclStmt(istream& in, int& line)
{
	LexItem t;
	bool status;
	
	t = Parser::GetNextToken(in, line);
	if(t == INT || t == FLOAT || t == BOOL)
	{
		status = VarList(in, line, t);
		
		if (!status)
		{
			ParseError(line, "Incorrect variable in Declaration Statement.");
			return status;
		}
	}
	else
	{
		ParseError(line, "Incorrect Declaration Type.");
		return false;
	}
	
	return true;
		
}//End of DeclStmt

//VarList:= Var {,Var}
bool VarList(istream& in, int& line, LexItem& type) {
	bool status = false;
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == IDENT)
	{
		//set IDENT lexeme to the type tok value
		identstr = tok.GetLexeme();
		if (cond) {
			if (!(defVar.find(identstr)->second))
			{
				defVar[identstr] = true;
				SymTable[identstr] = type.GetToken();
			}	
			else
			{
				ParseError(line, "Variable Redefinition");
				return false;
			}
		}
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = VarList(in, line, type);
	}
	else {
		Parser::PushBackToken(tok);
		return true;
	}
	return status;	
}//End of VarList
	

//ControlStmt ::= AssigStmt | IfStmt | PrintStmt 
bool ControlStmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		break;

	case IF:
		status = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		break;
		
	default:
		Parser::PushBackToken(t);
		return false;
	}

	return status;
}//End of ControlStmt


//PrintStmt:= PRINT (ExpreList) 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in PrintStmt" << endl;
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression list after Print");
		while(!(*ValQue).empty())
		{
			ValQue->pop();		
		}
		delete ValQue;
		return false;
	}
	
	//Evaluate: print out the list of expressions' values
	if (cond) {
		while (!(*ValQue).empty())
		{
			Value nextVal = (*ValQue).front();
			cout << nextVal;
			ValQue->pop();
		}
	cout << endl;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	return true;
}//End of PrintStmt

//IfStmt:= IF (Expr) then StmtList [Else StmtList] END IF
bool IfStmt(istream& in, int& line) {
	bool ex = false, status ; 
	LexItem t;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	Value retVal;
	ex = Expr(in, line, retVal);
	if (retVal.GetType() != VBOOL) {
		ParseError(line, "If statement must have a boolean expression");
		return false;
	}
	cond = retVal.GetBool();
	if( !ex ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != THEN)
	{
		ParseError(line, "If Statement Syntax Error");
		return false;
	}
	status = StmtList(in, line);
	if(!status)
	{
		ParseError(line, "Missing Statement for If-Stmt Then-Part");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (cond == false) cond = true;
	else cond = false;
	if( t == ELSE ) {
		status = StmtList(in, line);
		if(!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}
		else
		{
			t = Parser::GetNextToken(in, line);
		}
	}
	cond = true;
	
	if( t == END){
		t = Parser::GetNextToken(in, line);
		if(t != IF){
			ParseError(line, "Missing closing keywords of IF statement.");
			return false;
		}
		return true;
	}
	else
	{
		ParseError(line, "Missing END of IF.");
		return false;
	}
	
}//End of IfStmt function

//Var:= ident
bool Var(istream& in, int& line, LexItem & tok)
{
	string identstr;
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == IDENT){
		identstr = tok.GetLexeme();
		//cout<< "in assigstmt var: " << identstr << endl;
		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}	
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	
	return false;
}//End of Var

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {
	
	bool varstatus = false, status = false;
	LexItem t;
	LexItem idtk;
	varstatus = Var( in, line, idtk);
	Value retVal;
	if (varstatus){
		t = Parser::GetNextToken(in, line);
		
		if (t == ASSOP){
			status = Expr(in, line, retVal);
			
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
			
		} else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		} else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
		if (cond) {
			if (SymTable[idtk.GetLexeme()] == INT && retVal.GetType() == VINT) {
				TempsResults[idtk.GetLexeme()] = retVal;
			} else if (SymTable[idtk.GetLexeme()] == INT && retVal.GetType() == VREAL) {
				TempsResults[idtk.GetLexeme()] = Value((int)retVal.GetReal());
			} else if (SymTable[idtk.GetLexeme()] == FLOAT && retVal.GetType() == VREAL) {
				TempsResults[idtk.GetLexeme()] = retVal;
			} else if (SymTable[idtk.GetLexeme()] == FLOAT && retVal.GetType() == VINT) {
				TempsResults[idtk.GetLexeme()] =  Value((float)retVal.GetInt());
			} else if (SymTable[idtk.GetLexeme()] == BOOL && retVal.GetType() == VBOOL) {
				TempsResults[idtk.GetLexeme()] = retVal;
			} else {
				ParseError(line, "Type mismatch");
				return false;
			}
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;	
}//End of AssignStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;
	status = Expr(in, line, retVal);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		
		status = ExprList(in, line);
		
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList

//Expr ::= LogORExpr ::= LogANDExpr { || LogANDRxpr }
bool Expr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	LexItem tok;
	bool t1 = LogANDExpr(in, line, val1);
		
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == OR ) 
	{
		t1 = LogANDExpr(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		retVal = val1 || val2;
		if (retVal.IsErr() && cond) {
			ParseError(line, "Illegal OR operation.");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of Expr/LogORExpr

//LogANDExpr ::= EqualExpr { && EqualExpr }
bool LogANDExpr(istream& in, int& line, Value & retVal) {
	LexItem tok;
	Value val1, val2;
	bool t1 = EqualExpr(in, line, val1);
	//cout << "status of EqualExpr and val1: " << t1<< " " << val1.IsErr() << " " << val1.IsBool() << endl;
	//cout << "status of EqualExpr: " << t1<< endl;
		
	if( !t1 ) {
		return false;
	}
	//cout << "value of var1 in AND op " << val1.GetBool() << endl;
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == AND ) 
	{
		t1 = EqualExpr(in, line, val2);
		//cout << "value of var1 in AND op " << val1.GetBool() << endl;
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		//evaluate expression for Logical AND
		retVal = retVal && val2;
		//cout << "AND op result " << retVal.IsBool() << " " << retVal.GetBool() << endl;
		if(retVal.IsErr() && cond)
		{
			ParseError(line, "Illegal AND operation.");
			//cout << "(" << tok.GetLexeme() << ")" << endl;		
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	
	Parser::PushBackToken(tok);
	return true;
}//End of LogANDExpr


//EqualExpr ::= RelExpr [ == RelExpr ]
bool EqualExpr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	LexItem tok;
	bool t1 = RelExpr(in, line, val1);
		
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == EQUAL ) 
	{
		t1 = RelExpr(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		retVal = retVal == val2;
		if (retVal.IsErr() && cond) {
			ParseError(line, "Invalid equality expression");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if(tok == EQUAL)
		{
			ParseError(line, "Illegal Equality Expression.");
			return false;
		}
		else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of EqualExpr

//RelExpr ::= AddExpr [ ( < | > ) AddExpr ]
bool RelExpr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	LexItem tok;
	bool t1 = AddExpr(in, line, val1);
		
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == LTHAN || tok == GTHAN) 
	{
		t1 = AddExpr(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if (tok == LTHAN) retVal = retVal < val2;
		else retVal = retVal > val2;
		if (retVal.IsErr() && cond) {
			ParseError(line, "Invalid relational expression");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		
		if(tok == LTHAN || tok == GTHAN)
		{
			ParseError(line, "Illegal Relational Expression.");
			return false;
		}
		else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of RelExpr

//AddExpr :: MultExpr { ( + | - ) MultExpr }
bool AddExpr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	bool t1 = MultExpr(in, line, val1);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS ) 
	{
		t1 = MultExpr(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if (tok == PLUS) retVal = retVal + val2;
		else retVal = retVal - val1;
		if (retVal.IsErr() && cond) {
			ParseError(line, "Invalid use of + or -");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of MultExpr

//MultExpr ::= UnaryExpr { ( * | / ) UnaryExpr }
bool MultExpr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	bool t1 = UnaryExpr(in, line, val1);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  )
	{
		t1 = UnaryExpr(in, line, val2);
		
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if (tok == MULT) {
			retVal = retVal * val2;
		}
		else {
			if ((val2 == Value(0)).GetBool()) {
				ParseError(line, "Division by 0 error");
				retVal = Value();
				return false;
			} else {
				retVal = retVal / val2;
			}
		}
		if (retVal.IsErr() && cond) {
			ParseError(line, "Invalid use of * or /");
			retVal = Value();
			return false;
		}
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}//End of MultExpr

//UnaryExpr ::= ( - | + | ! ) PrimaryExpr | PrimaryExpr
bool UnaryExpr(istream& in, int& line, Value & retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	int sign = 0;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
	// added if for !
	else if (t == NOT) {
		sign = -2;
	} else
		Parser::PushBackToken(t);
		
	status = PrimaryExpr(in, line, sign, retVal);
	// implementation of -, +, and !
	if (sign == -1) {
		if (retVal.GetType() != VINT && retVal.GetType() != VREAL) {
			ParseError(line, "Invalid use of -");
			retVal = Value();
			return false;
		} else if (retVal.GetType() == VINT) {
			retVal = Value(-(retVal.GetInt()));
		} else {
			retVal = Value(-(retVal.GetReal()));
		}
	} else if (sign == 1) {
		if (retVal.GetType() != VINT && retVal.GetType() != VREAL) {
			ParseError(line, "Invalid use of +");
			retVal = Value();
			return false;
		}
	} else if (sign == -2) {
		if (retVal.GetType() != VBOOL) {
			ParseError(line, "Invalid use of !");
			retVal = Value();
			return false;
		} else {
			retVal = Value(!(retVal.GetBool()));
		}
	}
	return status;
}//End of UnaryExpr



//PrimaryExpr ::= IDENT | ICONST | RCONST | SCONST | BCONST | ( Expr )
bool PrimaryExpr(istream& in, int& line, int sign, Value & retVal) {
	
	LexItem tok = Parser::GetNextToken(in, line);
	

	if( tok == IDENT ) {
		
		string lexeme = tok.GetLexeme();
		if (cond) {
			if (!(defVar.find(lexeme)->second))
			{
				ParseError(line, "Using Undeclared Variable");
				return false;	
			}
			if (TempsResults.find(lexeme) == TempsResults.end()) {
				ParseError(line, "Undefined variable");
				return false;
			}
		}
		// if identifier, look up Value object in TempsResults and assign it to retVal
		retVal = TempsResults[lexeme];
		return true;
	}
	else if( tok == ICONST ) {
		// if integer constant, create value object for it, converting string to integer for constructor parameter
		retVal = Value(stoi(tok.GetLexeme()));
		return true;
	}
	else if( tok == SCONST ) {
		// if string constant, create value object for it, -passing string into constructor for Value class
		retVal = Value(tok.GetLexeme());
		return true;
	}
	else if( tok == RCONST ) {
		// if real constant, create value object for it, converting string to float for constructor parameter
		retVal = Value(stof(tok.GetLexeme()));
		return true;
	}
	else if( tok == BCONST ) {
		// if boolean constant, create value object for it based on lexeme
		if (tok.GetLexeme() == "TRUE") retVal = Value(true);
		else retVal = Value(false);
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
		if( !ex ) {
			ParseError(line, "Missing expression after Left Parenthesis");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing right Parenthesis after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}



