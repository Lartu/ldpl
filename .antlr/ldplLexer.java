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
public class ldplLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.7.1", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		MULTICOMMENT=1, COMMENT=2, WHITESPACE=3, NEWLINE=4, LINEFEED=5, CRLF=6, 
		OS_NAME=7, NUMBERTYPE=8, TEXTTYPE=9, LISTTYPE=10, MAPTYPE=11, RWOF=12, 
		RWIS=13, RWFLAG=14, RWINCLUDE=15, RWEXTENSION=16, RWSUBPR=17, RWDATA=18, 
		RWLOCAL=19, RWPROCEDURE=20, RWPARAMETERS=21, RWEND=22, RWCALL=23, RWWITH=24, 
		NUMBER=25, PLUS_OP=26, MINUS_OP=27, TIMES_OP=28, LPAR=29, RPAR=30, DIV_OP=31, 
		DQUOTE=32, DCHAR=33, ESCAPEDDQUOTE=34, STRING=35, IDENTIFIER=36;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	public static final String[] ruleNames = {
		"MULTICOMMENT", "COMMENT", "WHITESPACE", "NEWLINE", "LINEFEED", "CRLF", 
		"OS_NAME", "NUMBERTYPE", "TEXTTYPE", "LISTTYPE", "MAPTYPE", "RWOF", "RWIS", 
		"RWFLAG", "RWINCLUDE", "RWEXTENSION", "RWSUBPR", "RWDATA", "RWLOCAL", 
		"RWPROCEDURE", "RWPARAMETERS", "RWEND", "RWCALL", "RWWITH", "NUMBER", 
		"PLUS_OP", "MINUS_OP", "TIMES_OP", "LPAR", "RPAR", "DIV_OP", "DQUOTE", 
		"DCHAR", "ESCAPEDDQUOTE", "STRING", "IDENTIFIER"
	};

	private static final String[] _LITERAL_NAMES = {
		null, null, null, null, "'\n'", "'lf'", "'crlf'", null, null, null, null, 
		null, "'of'", "'is'", "'flag'", "'include'", "'extension'", null, "'data:'", 
		"'local'", "'procedure:'", "'parameters:'", "'end'", "'call'", "'with'", 
		null, "'+'", "'-'", "'*'", "'('", "')'", "'/'", "'\"'", null, "'\\\"'"
	};
	private static final String[] _SYMBOLIC_NAMES = {
		null, "MULTICOMMENT", "COMMENT", "WHITESPACE", "NEWLINE", "LINEFEED", 
		"CRLF", "OS_NAME", "NUMBERTYPE", "TEXTTYPE", "LISTTYPE", "MAPTYPE", "RWOF", 
		"RWIS", "RWFLAG", "RWINCLUDE", "RWEXTENSION", "RWSUBPR", "RWDATA", "RWLOCAL", 
		"RWPROCEDURE", "RWPARAMETERS", "RWEND", "RWCALL", "RWWITH", "NUMBER", 
		"PLUS_OP", "MINUS_OP", "TIMES_OP", "LPAR", "RPAR", "DIV_OP", "DQUOTE", 
		"DCHAR", "ESCAPEDDQUOTE", "STRING", "IDENTIFIER"
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


	public ldplLexer(CharStream input) {
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
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\2&\u0159\b\1\4\2\t"+
		"\2\4\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4\13"+
		"\t\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22\t\22"+
		"\4\23\t\23\4\24\t\24\4\25\t\25\4\26\t\26\4\27\t\27\4\30\t\30\4\31\t\31"+
		"\4\32\t\32\4\33\t\33\4\34\t\34\4\35\t\35\4\36\t\36\4\37\t\37\4 \t \4!"+
		"\t!\4\"\t\"\4#\t#\4$\t$\4%\t%\3\2\3\2\3\2\3\2\7\2P\n\2\f\2\16\2S\13\2"+
		"\3\2\3\2\3\2\3\2\3\2\3\3\3\3\7\3\\\n\3\f\3\16\3_\13\3\3\3\3\3\3\3\3\3"+
		"\3\4\3\4\3\4\3\4\3\5\3\5\3\6\3\6\3\6\3\7\3\7\3\7\3\7\3\7\3\b\3\b\3\b\3"+
		"\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b"+
		"\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\3\b\5\b\u0091\n\b\3\t\3\t\3\t\3\t\3\t"+
		"\3\t\3\t\3\t\3\t\3\t\3\t\3\t\3\t\5\t\u00a0\n\t\3\n\3\n\3\n\3\n\3\n\3\n"+
		"\3\n\3\n\3\n\5\n\u00ab\n\n\3\13\3\13\3\13\3\13\3\13\3\13\3\13\3\13\3\13"+
		"\5\13\u00b6\n\13\3\f\3\f\3\f\3\f\3\f\3\f\3\f\5\f\u00bf\n\f\3\r\3\r\3\r"+
		"\3\16\3\16\3\16\3\17\3\17\3\17\3\17\3\17\3\20\3\20\3\20\3\20\3\20\3\20"+
		"\3\20\3\20\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\22\3\22"+
		"\3\22\3\22\3\22\3\22\3\22\3\22\3\22\3\22\3\22\3\22\3\22\3\22\3\22\3\22"+
		"\5\22\u00ee\n\22\3\23\3\23\3\23\3\23\3\23\3\23\3\24\3\24\3\24\3\24\3\24"+
		"\3\24\3\25\3\25\3\25\3\25\3\25\3\25\3\25\3\25\3\25\3\25\3\25\3\26\3\26"+
		"\3\26\3\26\3\26\3\26\3\26\3\26\3\26\3\26\3\26\3\26\3\27\3\27\3\27\3\27"+
		"\3\30\3\30\3\30\3\30\3\30\3\31\3\31\3\31\3\31\3\31\3\32\5\32\u0122\n\32"+
		"\3\32\5\32\u0125\n\32\7\32\u0127\n\32\f\32\16\32\u012a\13\32\3\32\6\32"+
		"\u012d\n\32\r\32\16\32\u012e\3\32\3\32\6\32\u0133\n\32\r\32\16\32\u0134"+
		"\3\33\3\33\3\34\3\34\3\35\3\35\3\36\3\36\3\37\3\37\3 \3 \3!\3!\3\"\3\""+
		"\5\"\u0147\n\"\3#\3#\3#\3$\3$\7$\u014e\n$\f$\16$\u0151\13$\3$\3$\3%\6"+
		"%\u0156\n%\r%\16%\u0157\4Q]\2&\3\3\5\4\7\5\t\6\13\7\r\b\17\t\21\n\23\13"+
		"\25\f\27\r\31\16\33\17\35\20\37\21!\22#\23%\24\'\25)\26+\27-\30/\31\61"+
		"\32\63\33\65\34\67\359\36;\37= ?!A\"C#E$G%I&\3\2\6\5\2\13\13\17\17\"\""+
		"\3\2\62;\3\2$$\b\2$$*-//\61\61<<``\2\u016b\2\3\3\2\2\2\2\5\3\2\2\2\2\7"+
		"\3\2\2\2\2\t\3\2\2\2\2\13\3\2\2\2\2\r\3\2\2\2\2\17\3\2\2\2\2\21\3\2\2"+
		"\2\2\23\3\2\2\2\2\25\3\2\2\2\2\27\3\2\2\2\2\31\3\2\2\2\2\33\3\2\2\2\2"+
		"\35\3\2\2\2\2\37\3\2\2\2\2!\3\2\2\2\2#\3\2\2\2\2%\3\2\2\2\2\'\3\2\2\2"+
		"\2)\3\2\2\2\2+\3\2\2\2\2-\3\2\2\2\2/\3\2\2\2\2\61\3\2\2\2\2\63\3\2\2\2"+
		"\2\65\3\2\2\2\2\67\3\2\2\2\29\3\2\2\2\2;\3\2\2\2\2=\3\2\2\2\2?\3\2\2\2"+
		"\2A\3\2\2\2\2C\3\2\2\2\2E\3\2\2\2\2G\3\2\2\2\2I\3\2\2\2\3K\3\2\2\2\5Y"+
		"\3\2\2\2\7d\3\2\2\2\th\3\2\2\2\13j\3\2\2\2\rm\3\2\2\2\17\u0090\3\2\2\2"+
		"\21\u009f\3\2\2\2\23\u00aa\3\2\2\2\25\u00b5\3\2\2\2\27\u00be\3\2\2\2\31"+
		"\u00c0\3\2\2\2\33\u00c3\3\2\2\2\35\u00c6\3\2\2\2\37\u00cb\3\2\2\2!\u00d3"+
		"\3\2\2\2#\u00ed\3\2\2\2%\u00ef\3\2\2\2\'\u00f5\3\2\2\2)\u00fb\3\2\2\2"+
		"+\u0106\3\2\2\2-\u0112\3\2\2\2/\u0116\3\2\2\2\61\u011b\3\2\2\2\63\u0128"+
		"\3\2\2\2\65\u0136\3\2\2\2\67\u0138\3\2\2\29\u013a\3\2\2\2;\u013c\3\2\2"+
		"\2=\u013e\3\2\2\2?\u0140\3\2\2\2A\u0142\3\2\2\2C\u0146\3\2\2\2E\u0148"+
		"\3\2\2\2G\u014b\3\2\2\2I\u0155\3\2\2\2KL\7\61\2\2LM\7,\2\2MQ\3\2\2\2N"+
		"P\13\2\2\2ON\3\2\2\2PS\3\2\2\2QR\3\2\2\2QO\3\2\2\2RT\3\2\2\2SQ\3\2\2\2"+
		"TU\7,\2\2UV\7\61\2\2VW\3\2\2\2WX\b\2\2\2X\4\3\2\2\2Y]\7%\2\2Z\\\13\2\2"+
		"\2[Z\3\2\2\2\\_\3\2\2\2]^\3\2\2\2][\3\2\2\2^`\3\2\2\2_]\3\2\2\2`a\7\f"+
		"\2\2ab\3\2\2\2bc\b\3\2\2c\6\3\2\2\2de\t\2\2\2ef\3\2\2\2fg\b\4\2\2g\b\3"+
		"\2\2\2hi\7\f\2\2i\n\3\2\2\2jk\7n\2\2kl\7h\2\2l\f\3\2\2\2mn\7e\2\2no\7"+
		"t\2\2op\7n\2\2pq\7h\2\2q\16\3\2\2\2rs\7n\2\2st\7k\2\2tu\7p\2\2uv\7w\2"+
		"\2v\u0091\7z\2\2wx\7o\2\2xy\7c\2\2yz\7e\2\2z{\7q\2\2{\u0091\7u\2\2|}\7"+
		"c\2\2}~\7p\2\2~\177\7f\2\2\177\u0080\7t\2\2\u0080\u0081\7q\2\2\u0081\u0082"+
		"\7k\2\2\u0082\u0091\7f\2\2\u0083\u0084\7d\2\2\u0084\u0085\7u\2\2\u0085"+
		"\u0091\7f\2\2\u0086\u0087\7g\2\2\u0087\u0088\7o\2\2\u0088\u0089\7u\2\2"+
		"\u0089\u008a\7e\2\2\u008a\u008b\7t\2\2\u008b\u008c\7k\2\2\u008c\u008d"+
		"\7r\2\2\u008d\u008e\7v\2\2\u008e\u008f\7g\2\2\u008f\u0091\7p\2\2\u0090"+
		"r\3\2\2\2\u0090w\3\2\2\2\u0090|\3\2\2\2\u0090\u0083\3\2\2\2\u0090\u0086"+
		"\3\2\2\2\u0091\20\3\2\2\2\u0092\u0093\7p\2\2\u0093\u0094\7w\2\2\u0094"+
		"\u0095\7o\2\2\u0095\u0096\7d\2\2\u0096\u0097\7g\2\2\u0097\u00a0\7t\2\2"+
		"\u0098\u0099\7p\2\2\u0099\u009a\7w\2\2\u009a\u009b\7o\2\2\u009b\u009c"+
		"\7d\2\2\u009c\u009d\7g\2\2\u009d\u009e\7t\2\2\u009e\u00a0\7u\2\2\u009f"+
		"\u0092\3\2\2\2\u009f\u0098\3\2\2\2\u00a0\22\3\2\2\2\u00a1\u00a2\7v\2\2"+
		"\u00a2\u00a3\7g\2\2\u00a3\u00a4\7z\2\2\u00a4\u00ab\7v\2\2\u00a5\u00a6"+
		"\7v\2\2\u00a6\u00a7\7g\2\2\u00a7\u00a8\7z\2\2\u00a8\u00a9\7v\2\2\u00a9"+
		"\u00ab\7u\2\2\u00aa\u00a1\3\2\2\2\u00aa\u00a5\3\2\2\2\u00ab\24\3\2\2\2"+
		"\u00ac\u00ad\7n\2\2\u00ad\u00ae\7k\2\2\u00ae\u00af\7u\2\2\u00af\u00b6"+
		"\7v\2\2\u00b0\u00b1\7n\2\2\u00b1\u00b2\7k\2\2\u00b2\u00b3\7u\2\2\u00b3"+
		"\u00b4\7v\2\2\u00b4\u00b6\7u\2\2\u00b5\u00ac\3\2\2\2\u00b5\u00b0\3\2\2"+
		"\2\u00b6\26\3\2\2\2\u00b7\u00b8\7o\2\2\u00b8\u00b9\7c\2\2\u00b9\u00bf"+
		"\7r\2\2\u00ba\u00bb\7o\2\2\u00bb\u00bc\7c\2\2\u00bc\u00bd\7r\2\2\u00bd"+
		"\u00bf\7u\2\2\u00be\u00b7\3\2\2\2\u00be\u00ba\3\2\2\2\u00bf\30\3\2\2\2"+
		"\u00c0\u00c1\7q\2\2\u00c1\u00c2\7h\2\2\u00c2\32\3\2\2\2\u00c3\u00c4\7"+
		"k\2\2\u00c4\u00c5\7u\2\2\u00c5\34\3\2\2\2\u00c6\u00c7\7h\2\2\u00c7\u00c8"+
		"\7n\2\2\u00c8\u00c9\7c\2\2\u00c9\u00ca\7i\2\2\u00ca\36\3\2\2\2\u00cb\u00cc"+
		"\7k\2\2\u00cc\u00cd\7p\2\2\u00cd\u00ce\7e\2\2\u00ce\u00cf\7n\2\2\u00cf"+
		"\u00d0\7w\2\2\u00d0\u00d1\7f\2\2\u00d1\u00d2\7g\2\2\u00d2 \3\2\2\2\u00d3"+
		"\u00d4\7g\2\2\u00d4\u00d5\7z\2\2\u00d5\u00d6\7v\2\2\u00d6\u00d7\7g\2\2"+
		"\u00d7\u00d8\7p\2\2\u00d8\u00d9\7u\2\2\u00d9\u00da\7k\2\2\u00da\u00db"+
		"\7q\2\2\u00db\u00dc\7p\2\2\u00dc\"\3\2\2\2\u00dd\u00de\7u\2\2\u00de\u00df"+
		"\7w\2\2\u00df\u00ee\7d\2\2\u00e0\u00e1\7u\2\2\u00e1\u00e2\7w\2\2\u00e2"+
		"\u00e3\7d\2\2\u00e3\u00e4\7/\2\2\u00e4\u00e5\7r\2\2\u00e5\u00e6\7t\2\2"+
		"\u00e6\u00e7\7q\2\2\u00e7\u00e8\7e\2\2\u00e8\u00e9\7g\2\2\u00e9\u00ea"+
		"\7f\2\2\u00ea\u00eb\7w\2\2\u00eb\u00ec\7t\2\2\u00ec\u00ee\7g\2\2\u00ed"+
		"\u00dd\3\2\2\2\u00ed\u00e0\3\2\2\2\u00ee$\3\2\2\2\u00ef\u00f0\7f\2\2\u00f0"+
		"\u00f1\7c\2\2\u00f1\u00f2\7v\2\2\u00f2\u00f3\7c\2\2\u00f3\u00f4\7<\2\2"+
		"\u00f4&\3\2\2\2\u00f5\u00f6\7n\2\2\u00f6\u00f7\7q\2\2\u00f7\u00f8\7e\2"+
		"\2\u00f8\u00f9\7c\2\2\u00f9\u00fa\7n\2\2\u00fa(\3\2\2\2\u00fb\u00fc\7"+
		"r\2\2\u00fc\u00fd\7t\2\2\u00fd\u00fe\7q\2\2\u00fe\u00ff\7e\2\2\u00ff\u0100"+
		"\7g\2\2\u0100\u0101\7f\2\2\u0101\u0102\7w\2\2\u0102\u0103\7t\2\2\u0103"+
		"\u0104\7g\2\2\u0104\u0105\7<\2\2\u0105*\3\2\2\2\u0106\u0107\7r\2\2\u0107"+
		"\u0108\7c\2\2\u0108\u0109\7t\2\2\u0109\u010a\7c\2\2\u010a\u010b\7o\2\2"+
		"\u010b\u010c\7g\2\2\u010c\u010d\7v\2\2\u010d\u010e\7g\2\2\u010e\u010f"+
		"\7t\2\2\u010f\u0110\7u\2\2\u0110\u0111\7<\2\2\u0111,\3\2\2\2\u0112\u0113"+
		"\7g\2\2\u0113\u0114\7p\2\2\u0114\u0115\7f\2\2\u0115.\3\2\2\2\u0116\u0117"+
		"\7e\2\2\u0117\u0118\7c\2\2\u0118\u0119\7n\2\2\u0119\u011a\7n\2\2\u011a"+
		"\60\3\2\2\2\u011b\u011c\7y\2\2\u011c\u011d\7k\2\2\u011d\u011e\7v\2\2\u011e"+
		"\u011f\7j\2\2\u011f\62\3\2\2\2\u0120\u0122\7-\2\2\u0121\u0120\3\2\2\2"+
		"\u0121\u0122\3\2\2\2\u0122\u0124\3\2\2\2\u0123\u0125\7/\2\2\u0124\u0123"+
		"\3\2\2\2\u0124\u0125\3\2\2\2\u0125\u0127\3\2\2\2\u0126\u0121\3\2\2\2\u0127"+
		"\u012a\3\2\2\2\u0128\u0126\3\2\2\2\u0128\u0129\3\2\2\2\u0129\u012c\3\2"+
		"\2\2\u012a\u0128\3\2\2\2\u012b\u012d\t\3\2\2\u012c\u012b\3\2\2\2\u012d"+
		"\u012e\3\2\2\2\u012e\u012c\3\2\2\2\u012e\u012f\3\2\2\2\u012f\u0130\3\2"+
		"\2\2\u0130\u0132\7\60\2\2\u0131\u0133\t\3\2\2\u0132\u0131\3\2\2\2\u0133"+
		"\u0134\3\2\2\2\u0134\u0132\3\2\2\2\u0134\u0135\3\2\2\2\u0135\64\3\2\2"+
		"\2\u0136\u0137\7-\2\2\u0137\66\3\2\2\2\u0138\u0139\7/\2\2\u01398\3\2\2"+
		"\2\u013a\u013b\7,\2\2\u013b:\3\2\2\2\u013c\u013d\7*\2\2\u013d<\3\2\2\2"+
		"\u013e\u013f\7+\2\2\u013f>\3\2\2\2\u0140\u0141\7\61\2\2\u0141@\3\2\2\2"+
		"\u0142\u0143\7$\2\2\u0143B\3\2\2\2\u0144\u0147\n\4\2\2\u0145\u0147\5E"+
		"#\2\u0146\u0144\3\2\2\2\u0146\u0145\3\2\2\2\u0147D\3\2\2\2\u0148\u0149"+
		"\7^\2\2\u0149\u014a\7$\2\2\u014aF\3\2\2\2\u014b\u014f\5A!\2\u014c\u014e"+
		"\5C\"\2\u014d\u014c\3\2\2\2\u014e\u0151\3\2\2\2\u014f\u014d\3\2\2\2\u014f"+
		"\u0150\3\2\2\2\u0150\u0152\3\2\2\2\u0151\u014f\3\2\2\2\u0152\u0153\5A"+
		"!\2\u0153H\3\2\2\2\u0154\u0156\t\5\2\2\u0155\u0154\3\2\2\2\u0156\u0157"+
		"\3\2\2\2\u0157\u0155\3\2\2\2\u0157\u0158\3\2\2\2\u0158J\3\2\2\2\23\2Q"+
		"]\u0090\u009f\u00aa\u00b5\u00be\u00ed\u0121\u0124\u0128\u012e\u0134\u0146"+
		"\u014f\u0157\3\b\2\2";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}