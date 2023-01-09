#include "lex.h"
#include <map>
#include <string>
#include <cctype>
//maps keyword string to token
map<string,Token> kwmap = {
	{ "PROGRAM", PROGRAM},
	{ "PRINT", PRINT},
	{ "INT", INT},
	{ "END", END },
	{ "FLOAT", FLOAT},
	{ "ELSE", ELSE },
	{ "IF", IF },
	{ "THEN", THEN},
	{ "TRUE", TRUE},
	{ "FALSE", FALSE},
	{ "BOOL", BOOL }
};
//maps token to its string
map<Token,string> tokenPrint = {
	{PROGRAM, "PROGRAM"},
	{PRINT, "PRINT"},
	{INT, "INT"},
	{ END, "END" },
	{ FLOAT, "FLOAT"},
	{ BOOL, "BOOL" },
	
	{ ELSE, "ELSE" },
	{ IF, "IF" },
	
	{THEN, "THEN"},
	{ TRUE, "TRUE" },
	{ FALSE, "FALSE" },
	
	{ IDENT, "IDENT" },

	{ ICONST, "ICONST" },
	{ RCONST, "RCONST" },
	{ SCONST, "SCONST" },
	{ BCONST, "BCONST" },
	
	{ PLUS, "PLUS" },
	{ MINUS, "MINUS" },
	{ MULT, "MULT" },
	{ DIV, "DIV" },
	{ ASSOP, "ASSOP" },
	{ EQUAL, "EQUAL" },
	{ GTHAN, "GTHAN" },
	{ LTHAN, "LTHAN" },
	{ AND, "AND" },
	{ OR, "OR" },
	{ NOT, "NOT" },
        
	{ COMMA, "COMMA" },
	{ LPAREN, "LPAREN" },
	{ RPAREN, "RPAREN" },
	
	
	{ SEMICOL, "SEMICOL" },
	
	{ ERR, "ERR" },

	{ DONE, "DONE" },
};
//returns LexItem using lexeme and linenum
LexItem id_or_kw(const string& lexeme, int linenum) {
	Token tk;
	if (kwmap.find(lexeme) != kwmap.end()) {
		if (lexeme != "TRUE" && lexeme != "FALSE") {
			tk = kwmap[lexeme];
		} else {
			tk = BCONST;
		}
	} else {
		tk = IDENT;
	}
	LexItem li(tk, lexeme, linenum);
	return li;
}
//writes to and returns output stream using given output stream and LexItem object
ostream& operator<<(ostream& out, const LexItem& tok) {
	string tokenString = tokenPrint[tok.GetToken()];
	if (tok == IDENT || tok == ICONST || tok == RCONST || tok == SCONST || tok == BCONST) {
		if (tok == SCONST) {
			string lexeme = tok.GetLexeme();
			out << tokenString << "(" << lexeme.substr(1, lexeme.size() - 2) << ")";
		} else {
			out << tokenString << "(" << tok.GetLexeme() << ")";	
		}
	} else {
		out << tokenString; 
	}
	out << endl;
	return out;
}
//Error results in ERR lexitem being returned and lexeme value equal to string being recognized when error was detected
//we have to update linenumber
LexItem getNextToken (istream& in, int& linenumber) {
	enum TokState { START, INID, INSTRING, ININT, INREAL, INCOMMENT}
	lexstate = START;
	string lexeme;
	char ch;
	while (in.get(ch)) {
		switch (lexstate) {
			case START:
				if (ch == '\n') {
					linenumber++;
					continue;
				}
				if (isspace(ch)) {
					continue;
				}
				if (isalpha(ch) || ch == '_') {
					lexstate = INID;
					lexeme += toupper(ch);
					continue;
				}
				if (ch == '"') {
					lexstate = INSTRING;
					lexeme += ch;
					continue;
				}
				if (isdigit(ch)) {
					lexstate = ININT;
					lexeme += ch;
					continue;
				}
				if (ch == '/') {
					if (in.peek() == '*') {
						in.get(ch);
						lexstate = INCOMMENT;
						continue;
					} else {
						lexeme += ch;
						LexItem li(DIV, lexeme, linenumber);
						return li;
					}
				}
				if (ch == '+') {
					lexeme += ch;
					LexItem li(PLUS, lexeme, linenumber);
					return li;
				}
				if (ch == '-') {
					lexeme += ch;
					LexItem li(MINUS, lexeme, linenumber);
					return li;
				}
				if (ch == '*') {
					lexeme += ch;
					LexItem li(MULT, lexeme, linenumber);
					return li;
				}
				if (ch == '=') {
					lexeme += ch;
					if (in.peek() == '=') {
						in.get(ch);
						lexeme += ch;
						LexItem li(EQUAL, lexeme, linenumber);
						return li;
					} else {
						LexItem li(ASSOP, lexeme, linenumber);
						return li;
					}
				}
				if (ch == '(') {
					lexeme += ch;
					LexItem li(LPAREN, lexeme, linenumber);
					return li;
				}
				if (ch == ')') {
					lexeme += ch;
					LexItem li(RPAREN, lexeme, linenumber);
					return li;
				}
				if (ch == '>') {
					lexeme += ch;
					LexItem li(GTHAN, lexeme, linenumber);
					return li;
				}
				if (ch == '<') {
					lexeme += ch;
					LexItem li(LTHAN, lexeme, linenumber);
					return li;
				}
				if (ch == '&') {
					lexeme += ch;
					if (in.peek() == '&') {
						in.get(ch);
						lexeme += ch;
						LexItem li(AND, lexeme, linenumber);
						return li;
					} else {
						in.get(ch);
						lexeme += ch;
						LexItem li(ERR, lexeme, linenumber);
						return li;
					}
				}
				if (ch == '|') {
					lexeme += ch;
					if (in.peek() == '|') {
						in.get(ch);
						lexeme += ch;
						LexItem li(OR, lexeme, linenumber);
						return li;
					} else {
						in.get(ch);
						lexeme += ch;
						LexItem li(ERR, lexeme, linenumber);
						return li;
					}
				}
				if (ch == '!') {
					lexeme += ch;
					LexItem li(NOT, lexeme, linenumber);
					return li;
				} 
				if (ch == ',') {
					lexeme += ch;
					LexItem li(COMMA, lexeme, linenumber);
					return li;
				}
				if (ch == ';') {
					lexeme += ch;
					LexItem li(SEMICOL, lexeme, linenumber);
					return li;	
				} else {
					lexeme += ch;
					LexItem li(ERR, lexeme, linenumber);
					return li;	
				}
				break;
			case INID:
				if (ch == '\n') {
					in.putback(ch);
					LexItem li = id_or_kw(lexeme, linenumber);
					return li;
				}
				if (isspace(ch)) {					
					LexItem li = id_or_kw(lexeme, linenumber);
					return li;
				}
				if (in.eof()) {
					LexItem li = id_or_kw(lexeme, linenumber);
					return li;
				}
				if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '(' || ch == ')' || ch == '>' || ch == '<' || ch == '&' || ch == '|' || ch == '!' || ch == ',' || ch == ';') {
					in.putback(ch);					
					LexItem li = id_or_kw(lexeme, linenumber);
					return li;
				}
				if (!(isdigit(ch) || isalpha(ch) || ch == '_' || ch == '@')) {
					in.putback(ch);					
					LexItem li = id_or_kw(lexeme, linenumber);
					return li;
				}
				lexeme += toupper(ch);
				break;
			case INSTRING:
				if (ch == '\n') {
					in.putback(ch);
					LexItem li(ERR, lexeme, linenumber);
					return li;
				}
				if (in.eof()) {
					LexItem li(ERR, lexeme, linenumber);
					return li;
				}
				if (ch == '"') {
					lexeme += ch;
					LexItem li(SCONST, lexeme, linenumber);
					return li;
				}
				lexeme += ch;
				break;
			case ININT:
				if (ch == '.') {
					lexeme += ch;
					lexstate = INREAL;
					continue;
				}
				if (ch == '\n') {
					in.putback(ch);
					LexItem li(ICONST, lexeme, linenumber);
					return li;
				}
				if (isspace(ch)) {
					LexItem li(ICONST, lexeme, linenumber);
					return li;
				}
				if (in.eof()) {
					LexItem li(ICONST, lexeme, linenumber);
					return li;
				}
				if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '(' || ch == ')' || ch == '>' || ch == '<' || ch == '&' || ch == '|' || ch == '!' || ch == ',' || ch == ';'  || isalpha(ch) || ch == '_') {
					in.putback(ch);
					LexItem li(ICONST, lexeme, linenumber);
					return li;
				}
				if (!isdigit(ch)) {
					lexeme += ch;
					LexItem li(ERR, lexeme, linenumber);
					return li;
				}
				lexeme += ch;
				break;
			case INREAL:
				if (ch == '\n') {
					in.putback(ch);
					if (lexeme.back() != '.') {
						LexItem li(RCONST, lexeme, linenumber);
						return li;	
					} else {
						LexItem li(ERR, lexeme, linenumber);
						return li;
					}
				}
				if (isspace(ch)) {
					if (lexeme.back() != '.') {
						LexItem li(RCONST, lexeme, linenumber);
						return li;	
					} else {
						LexItem li(ERR, lexeme, linenumber);
						return li;
					}
				}
				if (in.eof()) {
					if (lexeme.back() != '.') {
						LexItem li(RCONST, lexeme, linenumber);
						return li;
					} else {
						LexItem li(ERR, lexeme, linenumber);
						return li;
					}
				}
				if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == '(' || ch == ')' || ch == '>' || ch == '<' || ch == '&' || ch == '|' || ch == '!' || ch == ',' || ch == ';' || isalpha(ch) || ch == '_') {
					in.putback(ch);
					if (lexeme.back() != '.') {
						LexItem li(RCONST, lexeme, linenumber);
						return li;
					} else {
						LexItem li(ERR, lexeme, linenumber);
						return li;
					}
				}
				if (!isdigit(ch)) {
					lexeme += ch;
					LexItem li(ERR, lexeme, linenumber);
					return li;
				}
				lexeme += ch;
				break;
			case INCOMMENT:
				if (ch == '\n') {
					linenumber++;
					continue;
				}
				if (ch == '*') {
					in.get(ch);
					if (ch == '/') {
						lexstate = START;
						continue;
					} else {
						in.putback(ch);
						continue;
					}
				}
		}
	}
	linenumber--;
	if (lexstate == INCOMMENT) {
		lexeme = "missing end delimiter";
		LexItem li(ERR, lexeme, linenumber);
		return li;
	}
	LexItem li(DONE, lexeme, linenumber);
	return li;
}

