//-----------------------------------------------------------------------
// Copyright 2011 Ciaran McHale.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions.
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.  
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//-----------------------------------------------------------------------
//
// BNF of config file
// ------------------
// Note: "|" denotes choice
//       "{ ... }*" denotes repetition 0+ times
//       "[ ... ]" denotes 0 or 1 times
//       "( ... )" denotes grouping
//
//  configFile = StmtList
//  StmtList   = { Stmt }*
//
//  Stmt = ident_sym ( '=' | '?=' | '+=' ) StringExpr ';'
//       | ident_sym ( '=' | '?=' | '+=' ) ListExpr ';'
//       | ident_sym '{' StmtList '}' [ ';' ]
//       | '@include' StringExpr [ '@ifExists' ] ';'
//       | '@copyFrom' ident_sym [ '@ifExists' ] ';'
//       | '@remove' ident_sym ';'
//       | '@error' StringExpr ';'
//       | '@if' '(' Condition ')' '{' StmtList '}'
//         { '@elseIf' '(' Condition ')' '{' StmtList '}' }*
//         [ '@else' '{' StmtList '}' ]
//         [ ';' ]
//
//  StringExpr = String { '+' String }*
//
//  String = string_sym
//         | ident_sym
//         | 'osType(' ')'
//         | 'osDirSeparator(' ')'
//         | 'osPathSeparator(' ')'
//         | 'getenv('  StringExpr [ ',' StringExpr ] ')'
//         | 'exec(' StringExpr [ ',' StringExpr ] ')'
//         | 'join(' ListExpr ',' StringExpr ')'
//         | 'siblingScope(' StringExpr ')'
//
//
//  ListExpr = List { '+' List }*
//  List     = '[' StringExprList [ ',' ] ']'
//           | ident_sym
//           | 'split(' StringExpr ',' StringExpr ')'
//
// StringExprList = empty
//                | StringExpr { ',' StringExpr }*
//
//  Condition     = OrCondition
//  OrCondition   = AndCondition { '||' AndCondition }*
//  AndCondition  = TermCondition { '&&' TermCondition }*
//  TermCondition = '(' Condition ')'
//                | '!' '(' Condition ')'
//                | 'isFileReadable(' StringExpr ')'
//                | StringExpr '==' StringExpr
//                | StringExpr '!=' StringExpr
//                | StringExpr '@in' ListExpr
//                | StringExpr '@matches' StringExpr
//----------------------------------------------------------------------


#ifndef CONFIG4CPP_CONFIG_PARSER_H_
#define CONFIG4CPP_CONFIG_PARSER_H_


//--------
// #include's
//--------
#include "ConfigLex.h"
#include "ConfigScope.h"
#include "ConfigurationImpl.h"


namespace CONFIG4CPP_NAMESPACE {

class ConfigParser {
public:
	//--------
	// Constructor and destructor
	//--------
	ConfigParser(
		Configuration::SourceType	sourceType,
		const char *				source,
		const char *				trustedCmdLine,
		const char *				sourceDescription,
		ConfigurationImpl *			config,
		bool						ifExistsIsSpecified = false);
	~ConfigParser();

	//--------
	// Public operations: None. All the work is done in the ctor!
	//--------

protected:
	//--------
	// Helper operations
	//--------
	void		parseStmtList();
	void		parseStmt();
	void		parseIncludeStmt();
	void		parseCopyStmt();
	void		parseRemoveStmt();
	void		parseErrorStmt();
	void		parseIfStmt();
	void		skipToClosingBrace();
	bool		parseCondition();
	bool		parseOrCondition();
	bool		parseAndCondition();
	bool		parseTerminalCondition();
	void		parseScope(LexToken & scopeName);
	void		parseRhsAssignStmt(LexToken & varName, short assignmentType);
	void		parseStringExpr(StringBuffer & expr);
	void		parseString(StringBuffer & expr);
	void		parseReadFile(StringBuffer & str);
	void		parseEnv(StringBuffer & str);
	void		parseSiblingScope(StringBuffer & str);
	void		parseExec(StringBuffer & str);
	void		parseJoin(StringBuffer & str);
	void		parseReplace(StringBuffer & str);
	void		parseSplit(StringVector & str);
	void		parseListExpr(StringVector & expr);
	void		parseList(StringVector & expr);
	void		parseStringExprList(StringVector & list);

	void		getDirectoryOfFile(const char * filename, StringBuffer & str);
	void		accept(short, const char *errMsg);
	void		error(const char *errMsg, bool printNear = true);

protected:
	//--------
	// Instance variables
	//--------
	ConfigLex *				m_lex;
	LexToken				m_token;
	ConfigurationImpl *		m_config;
	bool					m_errorInIncludedFile;
	StringBuffer			m_fileName;
};


}; // namespace CONFIG4CPP_NAMESPACE
#endif
