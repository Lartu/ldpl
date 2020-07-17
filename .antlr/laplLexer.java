// Generated from /home/martu/Desktop/Proyectos/2_Lenguajes/LDPL/ldpl/ldpl.g4 by ANTLR 4.7.1
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class laplLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.7.1", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		MULTICOMMENT=1, COMMENT=2, WHITESPACE=3, NEWLINE=4, EQ_OP=5, NEQ_OP=6, 
		LT_OP=7, GT_OP=8, LE_OP=9, GE_OP=10, PLUS_OP=11, MINUS_OP=12, POW_OP=13, 
		TIMES_OP=14, DIV_OP=15, MOD_OP=16, ASSIGN_OP=17, CONCAT_OP=18, LPAR=19, 
		RPAR=20, SEMICOLON=21, COLON=22, BLOCK_OPEN=23, BLOCK_CLOSE=24, INDEX_ACCESS_O=25, 
		INDEX_ACCESS_C=26, VARIABLE=27, COMMA=28, REF_OP=29, FLOOR_OP=30, CEIL_OP=31, 
		NOT_OP=32, OR_OP=33, AND_OP=34, STR_OP=35, NUM_OP=36, BOOLOP=37, WHILE=38, 
		CONTINUE=39, BREAK=40, IF=41, ELSE=42, DISPLAY=43, EXIT=44, TRUE=45, FALSE=46, 
		RETURN=47, FUNCTION=48, ARRAY=49, MAP=50, LINEFEED=51, CRLF=52, TYPE=53, 
		VAR=54, BIF_LEN=55, RANDOM=56, ACCEPT=57, BIF_ISNUM=58, BIF_REPLACE=59, 
		IDENTIFIER=60, NUMBER=61, QUOTE=62, DQUOTE=63, CHAR=64, DCHAR=65, ESCAPEDCHAR=66, 
		STRING=67;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	public static final String[] ruleNames = {
		"MULTICOMMENT", "COMMENT", "WHITESPACE", "NEWLINE", "EQ_OP", "NEQ_OP", 
		"LT_OP", "GT_OP", "LE_OP", "GE_OP", "PLUS_OP", "MINUS_OP", "POW_OP", "TIMES_OP", 
		"DIV_OP", "MOD_OP", "ASSIGN_OP", "CONCAT_OP", "LPAR", "RPAR", "SEMICOLON", 
		"COLON", "BLOCK_OPEN", "BLOCK_CLOSE", "INDEX_ACCESS_O", "INDEX_ACCESS_C", 
		"VARIABLE", "COMMA", "REF_OP", "FLOOR_OP", "CEIL_OP", "NOT_OP", "OR_OP", 
		"AND_OP", "STR_OP", "NUM_OP", "BOOLOP", "WHILE", "CONTINUE", "BREAK", 
		"IF", "ELSE", "DISPLAY", "EXIT", "TRUE", "FALSE", "RETURN", "FUNCTION", 
		"ARRAY", "MAP", "LINEFEED", "CRLF", "TYPE", "VAR", "BIF_LEN", "RANDOM", 
		"ACCEPT", "BIF_ISNUM", "BIF_REPLACE", "IDENTIFIER", "NUMBER", "QUOTE", 
		"DQUOTE", "CHAR", "DCHAR", "ESCAPEDCHAR", "STRING"
	};

	private static final String[] _LITERAL_NAMES = {
		null, null, null, null, "'\n'", "'=='", "'!='", "'<'", "'>'", "'<='", 
		"'>='", "'+'", "'-'", "'**'", "'*'", "'/'", "'%'", "'='", "'&'", "'('", 
		"')'", "';'", "':'", "'{'", "'}'", "'['", "']'", null, "','", "'ref'", 
		"'floor'", "'ceil'", "'not'", "'or'", "'and'", "'str'", "'num'", "'bool'", 
		"'while'", "'continue'", "'break'", "'if'", "'else'", "'display'", "'exit'", 
		"'true'", "'false'", "'return'", null, "'array'", "'map'", "'lf'", "'crlf'", 
		"'type'", "'var'", "'len'", "'random'", "'accept'", "'isNumeric'", "'replace'", 
		null, null, "'''", "'\"'"
	};
	private static final String[] _SYMBOLIC_NAMES = {
		null, "MULTICOMMENT", "COMMENT", "WHITESPACE", "NEWLINE", "EQ_OP", "NEQ_OP", 
		"LT_OP", "GT_OP", "LE_OP", "GE_OP", "PLUS_OP", "MINUS_OP", "POW_OP", "TIMES_OP", 
		"DIV_OP", "MOD_OP", "ASSIGN_OP", "CONCAT_OP", "LPAR", "RPAR", "SEMICOLON", 
		"COLON", "BLOCK_OPEN", "BLOCK_CLOSE", "INDEX_ACCESS_O", "INDEX_ACCESS_C", 
		"VARIABLE", "COMMA", "REF_OP", "FLOOR_OP", "CEIL_OP", "NOT_OP", "OR_OP", 
		"AND_OP", "STR_OP", "NUM_OP", "BOOLOP", "WHILE", "CONTINUE", "BREAK", 
		"IF", "ELSE", "DISPLAY", "EXIT", "TRUE", "FALSE", "RETURN", "FUNCTION", 
		"ARRAY", "MAP", "LINEFEED", "CRLF", "TYPE", "VAR", "BIF_LEN", "RANDOM", 
		"ACCEPT", "BIF_ISNUM", "BIF_REPLACE", "IDENTIFIER", "NUMBER", "QUOTE", 
		"DQUOTE", "CHAR", "DCHAR", "ESCAPEDCHAR", "STRING"
	};
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}


	public laplLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "ldpl.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\2E\u01c4\b\1\4\2\t"+
		"\2\4\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4\13"+
		"\t\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22\t\22"+
		"\4\23\t\23\4\24\t\24\4\25\t\25\4\26\t\26\4\27\t\27\4\30\t\30\4\31\t\31"+
		"\4\32\t\32\4\33\t\33\4\34\t\34\4\35\t\35\4\36\t\36\4\37\t\37\4 \t \4!"+
		"\t!\4\"\t\"\4#\t#\4$\t$\4%\t%\4&\t&\4\'\t\'\4(\t(\4)\t)\4*\t*\4+\t+\4"+
		",\t,\4-\t-\4.\t.\4/\t/\4\60\t\60\4\61\t\61\4\62\t\62\4\63\t\63\4\64\t"+
		"\64\4\65\t\65\4\66\t\66\4\67\t\67\48\t8\49\t9\4:\t:\4;\t;\4<\t<\4=\t="+
		"\4>\t>\4?\t?\4@\t@\4A\tA\4B\tB\4C\tC\4D\tD\3\2\3\2\3\2\3\2\7\2\u008e\n"+
		"\2\f\2\16\2\u0091\13\2\3\2\3\2\3\2\3\2\3\2\3\3\3\3\7\3\u009a\n\3\f\3\16"+
		"\3\u009d\13\3\3\3\3\3\3\3\3\3\3\4\3\4\3\4\3\4\3\5\3\5\3\6\3\6\3\6\3\7"+
		"\3\7\3\7\3\b\3\b\3\t\3\t\3\n\3\n\3\n\3\13\3\13\3\13\3\f\3\f\3\r\3\r\3"+
		"\16\3\16\3\16\3\17\3\17\3\20\3\20\3\21\3\21\3\22\3\22\3\23\3\23\3\24\3"+
		"\24\3\25\3\25\3\26\3\26\3\27\3\27\3\30\3\30\3\31\3\31\3\32\3\32\3\33\3"+
		"\33\3\34\3\34\3\34\7\34\u00dd\n\34\f\34\16\34\u00e0\13\34\3\35\3\35\3"+
		"\36\3\36\3\36\3\36\3\37\3\37\3\37\3\37\3\37\3\37\3 \3 \3 \3 \3 \3!\3!"+
		"\3!\3!\3\"\3\"\3\"\3#\3#\3#\3#\3$\3$\3$\3$\3%\3%\3%\3%\3&\3&\3&\3&\3&"+
		"\3\'\3\'\3\'\3\'\3\'\3\'\3(\3(\3(\3(\3(\3(\3(\3(\3(\3)\3)\3)\3)\3)\3)"+
		"\3*\3*\3*\3+\3+\3+\3+\3+\3,\3,\3,\3,\3,\3,\3,\3,\3-\3-\3-\3-\3-\3.\3."+
		"\3.\3.\3.\3/\3/\3/\3/\3/\3/\3\60\3\60\3\60\3\60\3\60\3\60\3\60\3\61\3"+
		"\61\3\61\3\61\3\61\3\61\5\61\u014d\n\61\3\62\3\62\3\62\3\62\3\62\3\62"+
		"\3\63\3\63\3\63\3\63\3\64\3\64\3\64\3\65\3\65\3\65\3\65\3\65\3\66\3\66"+
		"\3\66\3\66\3\66\3\67\3\67\3\67\3\67\38\38\38\38\39\39\39\39\39\39\39\3"+
		":\3:\3:\3:\3:\3:\3:\3;\3;\3;\3;\3;\3;\3;\3;\3;\3;\3<\3<\3<\3<\3<\3<\3"+
		"<\3<\3=\3=\7=\u0190\n=\f=\16=\u0193\13=\3>\6>\u0196\n>\r>\16>\u0197\3"+
		">\3>\6>\u019c\n>\r>\16>\u019d\5>\u01a0\n>\3?\3?\3@\3@\3A\3A\5A\u01a8\n"+
		"A\3B\3B\5B\u01ac\nB\3C\3C\3C\3D\3D\7D\u01b3\nD\fD\16D\u01b6\13D\3D\3D"+
		"\3D\3D\7D\u01bc\nD\fD\16D\u01bf\13D\3D\3D\5D\u01c3\nD\4\u008f\u009b\2"+
		"E\3\3\5\4\7\5\t\6\13\7\r\b\17\t\21\n\23\13\25\f\27\r\31\16\33\17\35\20"+
		"\37\21!\22#\23%\24\'\25)\26+\27-\30/\31\61\32\63\33\65\34\67\359\36;\37"+
		"= ?!A\"C#E$G%I&K\'M(O)Q*S+U,W-Y.[/]\60_\61a\62c\63e\64g\65i\66k\67m8o"+
		"9q:s;u<w=y>{?}@\177A\u0081B\u0083C\u0085D\u0087E\3\2\b\5\2\13\13\17\17"+
		"\"\"\5\2C\\aac|\6\2\62;C\\aac|\3\2\62;\3\2))\3\2$$\2\u01d0\2\3\3\2\2\2"+
		"\2\5\3\2\2\2\2\7\3\2\2\2\2\t\3\2\2\2\2\13\3\2\2\2\2\r\3\2\2\2\2\17\3\2"+
		"\2\2\2\21\3\2\2\2\2\23\3\2\2\2\2\25\3\2\2\2\2\27\3\2\2\2\2\31\3\2\2\2"+
		"\2\33\3\2\2\2\2\35\3\2\2\2\2\37\3\2\2\2\2!\3\2\2\2\2#\3\2\2\2\2%\3\2\2"+
		"\2\2\'\3\2\2\2\2)\3\2\2\2\2+\3\2\2\2\2-\3\2\2\2\2/\3\2\2\2\2\61\3\2\2"+
		"\2\2\63\3\2\2\2\2\65\3\2\2\2\2\67\3\2\2\2\29\3\2\2\2\2;\3\2\2\2\2=\3\2"+
		"\2\2\2?\3\2\2\2\2A\3\2\2\2\2C\3\2\2\2\2E\3\2\2\2\2G\3\2\2\2\2I\3\2\2\2"+
		"\2K\3\2\2\2\2M\3\2\2\2\2O\3\2\2\2\2Q\3\2\2\2\2S\3\2\2\2\2U\3\2\2\2\2W"+
		"\3\2\2\2\2Y\3\2\2\2\2[\3\2\2\2\2]\3\2\2\2\2_\3\2\2\2\2a\3\2\2\2\2c\3\2"+
		"\2\2\2e\3\2\2\2\2g\3\2\2\2\2i\3\2\2\2\2k\3\2\2\2\2m\3\2\2\2\2o\3\2\2\2"+
		"\2q\3\2\2\2\2s\3\2\2\2\2u\3\2\2\2\2w\3\2\2\2\2y\3\2\2\2\2{\3\2\2\2\2}"+
		"\3\2\2\2\2\177\3\2\2\2\2\u0081\3\2\2\2\2\u0083\3\2\2\2\2\u0085\3\2\2\2"+
		"\2\u0087\3\2\2\2\3\u0089\3\2\2\2\5\u0097\3\2\2\2\7\u00a2\3\2\2\2\t\u00a6"+
		"\3\2\2\2\13\u00a8\3\2\2\2\r\u00ab\3\2\2\2\17\u00ae\3\2\2\2\21\u00b0\3"+
		"\2\2\2\23\u00b2\3\2\2\2\25\u00b5\3\2\2\2\27\u00b8\3\2\2\2\31\u00ba\3\2"+
		"\2\2\33\u00bc\3\2\2\2\35\u00bf\3\2\2\2\37\u00c1\3\2\2\2!\u00c3\3\2\2\2"+
		"#\u00c5\3\2\2\2%\u00c7\3\2\2\2\'\u00c9\3\2\2\2)\u00cb\3\2\2\2+\u00cd\3"+
		"\2\2\2-\u00cf\3\2\2\2/\u00d1\3\2\2\2\61\u00d3\3\2\2\2\63\u00d5\3\2\2\2"+
		"\65\u00d7\3\2\2\2\67\u00d9\3\2\2\29\u00e1\3\2\2\2;\u00e3\3\2\2\2=\u00e7"+
		"\3\2\2\2?\u00ed\3\2\2\2A\u00f2\3\2\2\2C\u00f6\3\2\2\2E\u00f9\3\2\2\2G"+
		"\u00fd\3\2\2\2I\u0101\3\2\2\2K\u0105\3\2\2\2M\u010a\3\2\2\2O\u0110\3\2"+
		"\2\2Q\u0119\3\2\2\2S\u011f\3\2\2\2U\u0122\3\2\2\2W\u0127\3\2\2\2Y\u012f"+
		"\3\2\2\2[\u0134\3\2\2\2]\u0139\3\2\2\2_\u013f\3\2\2\2a\u014c\3\2\2\2c"+
		"\u014e\3\2\2\2e\u0154\3\2\2\2g\u0158\3\2\2\2i\u015b\3\2\2\2k\u0160\3\2"+
		"\2\2m\u0165\3\2\2\2o\u0169\3\2\2\2q\u016d\3\2\2\2s\u0174\3\2\2\2u\u017b"+
		"\3\2\2\2w\u0185\3\2\2\2y\u018d\3\2\2\2{\u0195\3\2\2\2}\u01a1\3\2\2\2\177"+
		"\u01a3\3\2\2\2\u0081\u01a7\3\2\2\2\u0083\u01ab\3\2\2\2\u0085\u01ad\3\2"+
		"\2\2\u0087\u01c2\3\2\2\2\u0089\u008a\7\61\2\2\u008a\u008b\7,\2\2\u008b"+
		"\u008f\3\2\2\2\u008c\u008e\13\2\2\2\u008d\u008c\3\2\2\2\u008e\u0091\3"+
		"\2\2\2\u008f\u0090\3\2\2\2\u008f\u008d\3\2\2\2\u0090\u0092\3\2\2\2\u0091"+
		"\u008f\3\2\2\2\u0092\u0093\7,\2\2\u0093\u0094\7\61\2\2\u0094\u0095\3\2"+
		"\2\2\u0095\u0096\b\2\2\2\u0096\4\3\2\2\2\u0097\u009b\7%\2\2\u0098\u009a"+
		"\13\2\2\2\u0099\u0098\3\2\2\2\u009a\u009d\3\2\2\2\u009b\u009c\3\2\2\2"+
		"\u009b\u0099\3\2\2\2\u009c\u009e\3\2\2\2\u009d\u009b\3\2\2\2\u009e\u009f"+
		"\7\f\2\2\u009f\u00a0\3\2\2\2\u00a0\u00a1\b\3\2\2\u00a1\6\3\2\2\2\u00a2"+
		"\u00a3\t\2\2\2\u00a3\u00a4\3\2\2\2\u00a4\u00a5\b\4\2\2\u00a5\b\3\2\2\2"+
		"\u00a6\u00a7\7\f\2\2\u00a7\n\3\2\2\2\u00a8\u00a9\7?\2\2\u00a9\u00aa\7"+
		"?\2\2\u00aa\f\3\2\2\2\u00ab\u00ac\7#\2\2\u00ac\u00ad\7?\2\2\u00ad\16\3"+
		"\2\2\2\u00ae\u00af\7>\2\2\u00af\20\3\2\2\2\u00b0\u00b1\7@\2\2\u00b1\22"+
		"\3\2\2\2\u00b2\u00b3\7>\2\2\u00b3\u00b4\7?\2\2\u00b4\24\3\2\2\2\u00b5"+
		"\u00b6\7@\2\2\u00b6\u00b7\7?\2\2\u00b7\26\3\2\2\2\u00b8\u00b9\7-\2\2\u00b9"+
		"\30\3\2\2\2\u00ba\u00bb\7/\2\2\u00bb\32\3\2\2\2\u00bc\u00bd\7,\2\2\u00bd"+
		"\u00be\7,\2\2\u00be\34\3\2\2\2\u00bf\u00c0\7,\2\2\u00c0\36\3\2\2\2\u00c1"+
		"\u00c2\7\61\2\2\u00c2 \3\2\2\2\u00c3\u00c4\7\'\2\2\u00c4\"\3\2\2\2\u00c5"+
		"\u00c6\7?\2\2\u00c6$\3\2\2\2\u00c7\u00c8\7(\2\2\u00c8&\3\2\2\2\u00c9\u00ca"+
		"\7*\2\2\u00ca(\3\2\2\2\u00cb\u00cc\7+\2\2\u00cc*\3\2\2\2\u00cd\u00ce\7"+
		"=\2\2\u00ce,\3\2\2\2\u00cf\u00d0\7<\2\2\u00d0.\3\2\2\2\u00d1\u00d2\7}"+
		"\2\2\u00d2\60\3\2\2\2\u00d3\u00d4\7\177\2\2\u00d4\62\3\2\2\2\u00d5\u00d6"+
		"\7]\2\2\u00d6\64\3\2\2\2\u00d7\u00d8\7_\2\2\u00d8\66\3\2\2\2\u00d9\u00da"+
		"\7&\2\2\u00da\u00de\t\3\2\2\u00db\u00dd\t\4\2\2\u00dc\u00db\3\2\2\2\u00dd"+
		"\u00e0\3\2\2\2\u00de\u00dc\3\2\2\2\u00de\u00df\3\2\2\2\u00df8\3\2\2\2"+
		"\u00e0\u00de\3\2\2\2\u00e1\u00e2\7.\2\2\u00e2:\3\2\2\2\u00e3\u00e4\7t"+
		"\2\2\u00e4\u00e5\7g\2\2\u00e5\u00e6\7h\2\2\u00e6<\3\2\2\2\u00e7\u00e8"+
		"\7h\2\2\u00e8\u00e9\7n\2\2\u00e9\u00ea\7q\2\2\u00ea\u00eb\7q\2\2\u00eb"+
		"\u00ec\7t\2\2\u00ec>\3\2\2\2\u00ed\u00ee\7e\2\2\u00ee\u00ef\7g\2\2\u00ef"+
		"\u00f0\7k\2\2\u00f0\u00f1\7n\2\2\u00f1@\3\2\2\2\u00f2\u00f3\7p\2\2\u00f3"+
		"\u00f4\7q\2\2\u00f4\u00f5\7v\2\2\u00f5B\3\2\2\2\u00f6\u00f7\7q\2\2\u00f7"+
		"\u00f8\7t\2\2\u00f8D\3\2\2\2\u00f9\u00fa\7c\2\2\u00fa\u00fb\7p\2\2\u00fb"+
		"\u00fc\7f\2\2\u00fcF\3\2\2\2\u00fd\u00fe\7u\2\2\u00fe\u00ff\7v\2\2\u00ff"+
		"\u0100\7t\2\2\u0100H\3\2\2\2\u0101\u0102\7p\2\2\u0102\u0103\7w\2\2\u0103"+
		"\u0104\7o\2\2\u0104J\3\2\2\2\u0105\u0106\7d\2\2\u0106\u0107\7q\2\2\u0107"+
		"\u0108\7q\2\2\u0108\u0109\7n\2\2\u0109L\3\2\2\2\u010a\u010b\7y\2\2\u010b"+
		"\u010c\7j\2\2\u010c\u010d\7k\2\2\u010d\u010e\7n\2\2\u010e\u010f\7g\2\2"+
		"\u010fN\3\2\2\2\u0110\u0111\7e\2\2\u0111\u0112\7q\2\2\u0112\u0113\7p\2"+
		"\2\u0113\u0114\7v\2\2\u0114\u0115\7k\2\2\u0115\u0116\7p\2\2\u0116\u0117"+
		"\7w\2\2\u0117\u0118\7g\2\2\u0118P\3\2\2\2\u0119\u011a\7d\2\2\u011a\u011b"+
		"\7t\2\2\u011b\u011c\7g\2\2\u011c\u011d\7c\2\2\u011d\u011e\7m\2\2\u011e"+
		"R\3\2\2\2\u011f\u0120\7k\2\2\u0120\u0121\7h\2\2\u0121T\3\2\2\2\u0122\u0123"+
		"\7g\2\2\u0123\u0124\7n\2\2\u0124\u0125\7u\2\2\u0125\u0126\7g\2\2\u0126"+
		"V\3\2\2\2\u0127\u0128\7f\2\2\u0128\u0129\7k\2\2\u0129\u012a\7u\2\2\u012a"+
		"\u012b\7r\2\2\u012b\u012c\7n\2\2\u012c\u012d\7c\2\2\u012d\u012e\7{\2\2"+
		"\u012eX\3\2\2\2\u012f\u0130\7g\2\2\u0130\u0131\7z\2\2\u0131\u0132\7k\2"+
		"\2\u0132\u0133\7v\2\2\u0133Z\3\2\2\2\u0134\u0135\7v\2\2\u0135\u0136\7"+
		"t\2\2\u0136\u0137\7w\2\2\u0137\u0138\7g\2\2\u0138\\\3\2\2\2\u0139\u013a"+
		"\7h\2\2\u013a\u013b\7c\2\2\u013b\u013c\7n\2\2\u013c\u013d\7u\2\2\u013d"+
		"\u013e\7g\2\2\u013e^\3\2\2\2\u013f\u0140\7t\2\2\u0140\u0141\7g\2\2\u0141"+
		"\u0142\7v\2\2\u0142\u0143\7w\2\2\u0143\u0144\7t\2\2\u0144\u0145\7p\2\2"+
		"\u0145`\3\2\2\2\u0146\u0147\7h\2\2\u0147\u0148\7w\2\2\u0148\u014d\7p\2"+
		"\2\u0149\u014a\7f\2\2\u014a\u014b\7g\2\2\u014b\u014d\7h\2\2\u014c\u0146"+
		"\3\2\2\2\u014c\u0149\3\2\2\2\u014db\3\2\2\2\u014e\u014f\7c\2\2\u014f\u0150"+
		"\7t\2\2\u0150\u0151\7t\2\2\u0151\u0152\7c\2\2\u0152\u0153\7{\2\2\u0153"+
		"d\3\2\2\2\u0154\u0155\7o\2\2\u0155\u0156\7c\2\2\u0156\u0157\7r\2\2\u0157"+
		"f\3\2\2\2\u0158\u0159\7n\2\2\u0159\u015a\7h\2\2\u015ah\3\2\2\2\u015b\u015c"+
		"\7e\2\2\u015c\u015d\7t\2\2\u015d\u015e\7n\2\2\u015e\u015f\7h\2\2\u015f"+
		"j\3\2\2\2\u0160\u0161\7v\2\2\u0161\u0162\7{\2\2\u0162\u0163\7r\2\2\u0163"+
		"\u0164\7g\2\2\u0164l\3\2\2\2\u0165\u0166\7x\2\2\u0166\u0167\7c\2\2\u0167"+
		"\u0168\7t\2\2\u0168n\3\2\2\2\u0169\u016a\7n\2\2\u016a\u016b\7g\2\2\u016b"+
		"\u016c\7p\2\2\u016cp\3\2\2\2\u016d\u016e\7t\2\2\u016e\u016f\7c\2\2\u016f"+
		"\u0170\7p\2\2\u0170\u0171\7f\2\2\u0171\u0172\7q\2\2\u0172\u0173\7o\2\2"+
		"\u0173r\3\2\2\2\u0174\u0175\7c\2\2\u0175\u0176\7e\2\2\u0176\u0177\7e\2"+
		"\2\u0177\u0178\7g\2\2\u0178\u0179\7r\2\2\u0179\u017a\7v\2\2\u017at\3\2"+
		"\2\2\u017b\u017c\7k\2\2\u017c\u017d\7u\2\2\u017d\u017e\7P\2\2\u017e\u017f"+
		"\7w\2\2\u017f\u0180\7o\2\2\u0180\u0181\7g\2\2\u0181\u0182\7t\2\2\u0182"+
		"\u0183\7k\2\2\u0183\u0184\7e\2\2\u0184v\3\2\2\2\u0185\u0186\7t\2\2\u0186"+
		"\u0187\7g\2\2\u0187\u0188\7r\2\2\u0188\u0189\7n\2\2\u0189\u018a\7c\2\2"+
		"\u018a\u018b\7e\2\2\u018b\u018c\7g\2\2\u018cx\3\2\2\2\u018d\u0191\t\3"+
		"\2\2\u018e\u0190\t\4\2\2\u018f\u018e\3\2\2\2\u0190\u0193\3\2\2\2\u0191"+
		"\u018f\3\2\2\2\u0191\u0192\3\2\2\2\u0192z\3\2\2\2\u0193\u0191\3\2\2\2"+
		"\u0194\u0196\t\5\2\2\u0195\u0194\3\2\2\2\u0196\u0197\3\2\2\2\u0197\u0195"+
		"\3\2\2\2\u0197\u0198\3\2\2\2\u0198\u019f\3\2\2\2\u0199\u019b\7\60\2\2"+
		"\u019a\u019c\t\5\2\2\u019b\u019a\3\2\2\2\u019c\u019d\3\2\2\2\u019d\u019b"+
		"\3\2\2\2\u019d\u019e\3\2\2\2\u019e\u01a0\3\2\2\2\u019f\u0199\3\2\2\2\u019f"+
		"\u01a0\3\2\2\2\u01a0|\3\2\2\2\u01a1\u01a2\7)\2\2\u01a2~\3\2\2\2\u01a3"+
		"\u01a4\7$\2\2\u01a4\u0080\3\2\2\2\u01a5\u01a8\n\6\2\2\u01a6\u01a8\5\u0085"+
		"C\2\u01a7\u01a5\3\2\2\2\u01a7\u01a6\3\2\2\2\u01a8\u0082\3\2\2\2\u01a9"+
		"\u01ac\n\7\2\2\u01aa\u01ac\5\u0085C\2\u01ab\u01a9\3\2\2\2\u01ab\u01aa"+
		"\3\2\2\2\u01ac\u0084\3\2\2\2\u01ad\u01ae\7^\2\2\u01ae\u01af\13\2\2\2\u01af"+
		"\u0086\3\2\2\2\u01b0\u01b4\5}?\2\u01b1\u01b3\5\u0081A\2\u01b2\u01b1\3"+
		"\2\2\2\u01b3\u01b6\3\2\2\2\u01b4\u01b2\3\2\2\2\u01b4\u01b5\3\2\2\2\u01b5"+
		"\u01b7\3\2\2\2\u01b6\u01b4\3\2\2\2\u01b7\u01b8\5}?\2\u01b8\u01c3\3\2\2"+
		"\2\u01b9\u01bd\5\177@\2\u01ba\u01bc\5\u0083B\2\u01bb\u01ba\3\2\2\2\u01bc"+
		"\u01bf\3\2\2\2\u01bd\u01bb\3\2\2\2\u01bd\u01be\3\2\2\2\u01be\u01c0\3\2"+
		"\2\2\u01bf\u01bd\3\2\2\2\u01c0\u01c1\5\177@\2\u01c1\u01c3\3\2\2\2\u01c2"+
		"\u01b0\3\2\2\2\u01c2\u01b9\3\2\2\2\u01c3\u0088\3\2\2\2\20\2\u008f\u009b"+
		"\u00de\u014c\u0191\u0197\u019d\u019f\u01a7\u01ab\u01b4\u01bd\u01c2\3\b"+
		"\2\2";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}