// This file was generated by lezer-generator. You probably shouldn't edit it.
import {LRParser} from "@lezer/lr"
import {highlighting} from "./pixel-highlight.js"
const spec_Identifier = {__proto__:null,let:14, not:56, __randi:58, __width:60, __height:62, __read:64, __newarr:70, and:74, or:78, __print:88, __delay:92, __pixelr:96, __pixel:98, if:102, else:110, for:114, while:118, return:122, fun:126}
export const parser = LRParser.deserialize({
  version: 14,
  states: "1vQ]QPOOOOQO'#C}'#C}O!QQPO'#DWO!YQPO'#DbO!aQPO'#C`OOQO'#C`'#C`OOQO'#Dq'#DqQ]QPOOO!fQPO'#CaO!kQPO'#DXO!kQPO'#DZO!kQPO'#D]O#`QPO'#D`O#eQPO'#DfO#jQPO'#DhO!kQPO'#DjO#oQPO'#DlO!kQPO,59iO!kQPO,59rOOQO,59|,59|O#tQPO,59|OOQO,58z,58zOOQO-E7o-E7oO#{QPO,58{O$QQPO'#C}O!kQPO'#CvO$]QPO'#CmOOQO'#Cm'#CmO!kQPO'#CmO$eQQO'#ClO%VQQO'#CkO%qQQO'#CjOOQO,59s,59sOOQO,59u,59uO&VQPO,59wO!kQPO,59zO&[QPO,5:QO!kQPO,5:SOOQO,5:U,5:UO&dQPO,5:WO&iQPO1G/TOOQO1G/^1G/^OOQO1G/h1G/hO&nQPO1G.gO&sQPO,59]O&zQPO,59bOOQO,59X,59XO&nQPO'#DOO!kQPO,59WO!kQPO,59VO!kQPO,59UO!kQPO1G/cO'PQPO1G/fO!kQPO1G/lO'UQPO1G/lO'ZQPO1G/nO'`QPO1G/rOOQO7+$o7+$oO'hQPO'#CeO'mQPO7+$RO'rQPO'#CsOOQO1G.w1G.wO'zQPO1G.wOOQO1G.|1G.|O(PQPO,59jOOQO1G.r1G.rOOQO1G.q1G.qOOQO1G.p1G.pO(UQPO7+$}O(ZQPO7+%QO(`QPO7+%WO!kQPO7+%WO(ZQPO7+%YO(eQPO'#DoO(jQPO'#DnO(rQPO7+%^O(wQPO7+%^O(|QPO,59PO!kQPO<<GmO!kQPO'#DrO)RQPO,59_OOQO7+$c7+$cO!kQPO1G/UO!kQPO<<HiO)ZQPO<<HlO*XQPO<<HrO*aQPO<<HrOOQO<<Ht<<HtO&nQPO,5:ZO*fQPO'#DsO*kQPO,5:YO&nQPO<<HxO*sQPO<<HxO&nQPO1G.kOOQOAN=XAN=XOOQO,5:^,5:^OOQO-E7p-E7pOOQO7+$p7+$pO*xQPOAN>TO(ZQPOAN>WO(ZQPOAN>^O*}QPOAN>^O+SQPOAN>^OOQO1G/u1G/uOOQO,5:_,5:_OOQO-E7q-E7qO(ZQPOAN>dO&nQPOAN>dOOQO7+$V7+$VO!kQPOG23oOOQOG23rG23rOOQOG23xG23xO(ZQPOG23xO+[QPOG23xOOQOG24OG24OO(ZQPOG24OO+aQPOLD)ZOOQOLD)dLD)dO(ZQPOLD)dOOQOLD)jLD)jO!kQPO!$'LuOOQO!$'MO!$'MOO+fQPO!)9BaO!kQPO!.K7{O+kQPO!4/-gO!kQPO!9A#RO+pQPO!?$FmO!kQPO!D6<XOOQO!IH1s!IH1s",
  stateData: ",Q~O!jOSPOSQOS~OUPOVWO|XO!OYO!QZO!T[O!VRO!Z]O!]^O!__O!a`O~OZaO]bO~O!WcO~P]OyeO~OUgO~OUhObkOckOdkOfiOklOllOmlOnkOokOplO~OfsO~OftO~OfuO~OUwO~O!WzO~P]OW{O~Of|OZqXsqX~OZaOs!PO~Ot!QOu!QOv`Xw`Xx`Xy`Xh`X[`Xi`X!R`X~Ov!ROw!ROx_Xy_Xh_X[_Xi_X!R_X~Ox!SOy^Xh^X[^Xi^X!R^X~Oh!TO~OVWOy!VO~Oi!YO~O[!ZO~OY![O~Oi!_O~P!kOi!aO~Oi!gO~Oy!iO~Oi!jO~OU!kOf!mO~OZ!oO~O]!pO~Oh!qOigX~Oi!sO~Oh!tO~Oh!uO~O!VRO~Oy!wO~OW!zO~Oh!{Of!bX~O!d!}O~Of#OO~O[#PO~Oh!qOiga~O!X#VOU!SyV!Sy|!Sy!O!Sy!Q!Sy!T!Sy!V!Sy!Z!Sy!]!Sy!_!Sy!a!Sy!h!Sy!W!Sy~OUPOi#WO~Oy#YO~OU!kO~Oh!{Of!ba~O!d#_O~Oh#aO~Oi#dO~OUPOi#dO~Oi#jO~Oh#lO~O!R#oO~Oh#qO~Oh#sO~OPQ!dYbtvkUb~",
  goto: ")|!hPPPP!i!oPPP!xPPPP#[$p%]%yPPP&kP'XPP&kPPPPPP'[(QPPPPPPP(T(aP(aP(aPP(gP(mPPP(gP(gP(aP(gP)])`P)f)p)vXUORVdWSORVdR!WtQ!]{Q!b!PQ#Z!zQ#^!}Q#`#PR#g#_QpXQqYQrZQv_QxaQybQ}iQ!UsQ!XuQ!^|Q!e!SQ!f!TQ!h!VQ!x!iQ#Q!pQ#R!qQ#U!uQ#h#aQ#n#lQ#p#oQ#r#qR#t#s|oXYZ_abisu|!S!T!V!i!p!q!u#a#l#o#q#sR!d!R!OnXYZ_abisu|!R!S!T!V!i!p!q!u#a#l#o#q#sR!c!Q!QmXYZ_abisu|!Q!R!S!T!V!i!p!q!u#a#l#o#q#sQ!OlR#T!t!VkXYZ_abilsu|!Q!R!S!T!V!i!p!q!t!u#a#l#o#q#sR!`|[QORVd!w#Y!VjXYZ_abilsu|!Q!R!S!T!V!i!p!q!t!u#a#l#o#q#sR!OjWSORVdQ#X!wR#e#YXSORVdXTORVdWTORVdQ!v!gQ!y!jQ#b#VQ#c#WQ#f#^Q#i#dQ#k#gR#m#jR!n!YQ!l!YR#[!{QVOQdRTfVdQ!r!^R#S!rQ!|!lR#]!|",
  nodeNames: "⚠ LineComment BlockComment PixelProgram Statement VariableDecl Identifier let Colon Typename SimpleTypename LeftSqBracket RightSqBracket Equals Expr SimpleExpr Term Factor BoolLiteral Number Colour FunctionCall LeftBracket ActualParams Comma RightBracket SubExpr UnaryOp not __randi __width __height __read LValue NewArrExpr __newarr MultiplicativeOp and AdditiveOp or RelationalOp Semicolon Assignment PrintStatement __print DelayStatement __delay PixelStatement __pixelr __pixel IfStatement if Block LeftBrace RightBrace else ForStatement for WhileStatement while RtrnStatement return FunctionDecl fun FormalParams FormalParam Arrow",
  maxTerm: 72,
  propSources: [highlighting],
  skippedNodes: [0,1,2],
  repeatNodeCount: 3,
  tokenData: "4j~RzX^#upq#uqr$jst$uxy%ayz%fz{%k{|%p|}%u}!O%z!O!P&Z!P!Q&i!Q![)P![!])[!]!^)a!^!_)f!_!`)n!`!a)f!c!})v!}#O*h#P#Q*m#R#S)v#T#U)v#U#V*r#V#W,l#W#Y)v#Y#Z/R#Z#])v#]#^2{#^#h)v#h#i3b#i#o)v#o#p4`#q#r4e#y#z#u$f$g#u#BY#BZ#u$IS$I_#u$I|$JO#u$JT$JU#u$KV$KW#u&FU&FV#u~#zY!j~X^#upq#u#y#z#u$f$g#u#BY#BZ#u$IS$I_#u$I|$JO#u$JT$JU#u$KV$KW#u&FU&FV#uQ$mP!_!`$pQ$uOxQ~$xR!Q![%R!c!i%R#T#Z%R~%WRd~!Q![%R!c!i%R#T#Z%R~%fOf~~%kOi~~%pOt~~%uOv~~%zOh~~&RPv~k~!`!a&U~&ZO!d~~&^P!Q![&a~&fPc~!Q![&a~&nQt~z{&t!P!Q(h~&wTOz'Wz{(X{;'S'W;'S;=`(R<%lO'W~'ZTOz'Wz{'j{;'S'W;'S;=`(R<%lO'W~'mTO!P'W!P!Q'|!Q;'S'W;'S;=`(R<%lO'W~(ROQ~~(UP;=`<%l'W~([SO!P'W!Q;'S'W;'S;=`(R<%lO'W~(mSP~OY(hZ;'S(h;'S;=`(y<%lO(h~(|P;=`<%l(h~)UQc~!O!P&Z!Q![)P~)aOW~~)fOy~Q)kPxQ!_!`$pR)sP]P!_!`$p~)yS!Q![*V!c!}*V#R#S*V#T#o*V~*[SU~!Q![*V!c!}*V#R#S*V#T#o*V~*mOZ~~*rO[~~*uU!Q![*V!c!}*V#R#S*V#T#c*V#c#d+X#d#o*V~+^UU~!Q![*V!c!}*V#R#S*V#T#c*V#c#d+p#d#o*V~+uUU~!Q![*V!c!}*V#R#S*V#T#`*V#`#a,X#a#o*V~,`SY~U~!Q![*V!c!}*V#R#S*V#T#o*V~,oU!Q![*V!c!}*V#R#S*V#T#c*V#c#d-R#d#o*V~-WUU~!Q![*V!c!}*V#R#S*V#T#`*V#`#a-j#a#o*V~-oUU~!Q![*V!c!}*V#R#S*V#T#c*V#c#d.R#d#o*V~.WUU~!Q![*V!c!}*V#R#S*V#T#i*V#i#j.j#j#o*V~.oUU~!Q![*V!c!}*V#R#S*V#T#f*V#f#g,X#g#o*V~/UV!Q![*V!c!}*V#R#S*V#T#U/k#U#`*V#`#a1g#a#o*V~/pUU~!Q![*V!c!}*V#R#S*V#T#`*V#`#a0S#a#o*V~0XUU~!Q![*V!c!}*V#R#S*V#T#g*V#g#h0k#h#o*V~0pUU~!Q![*V!c!}*V#R#S*V#T#X*V#X#Y1S#Y#o*V~1ZSb~U~!Q![*V!c!}*V#R#S*V#T#o*V~1lUU~!Q![*V!c!}*V#R#S*V#T#c*V#c#d2O#d#o*V~2TTU~!Q![*V!c!}*V#R#S*V#T#U2d#U#o*V~2iUU~!Q![*V!c!}*V#R#S*V#T#h*V#h#i,X#i#o*V~3OU!Q![*V!c!}*V#R#S*V#T#b*V#b#c2d#c#o*V~3eU!Q![*V!c!}*V#R#S*V#T#f*V#f#g3w#g#o*V~3|UU~!Q![*V!c!}*V#R#S*V#T#i*V#i#j0k#j#o*V~4eO!V~~4jO!W~",
  tokenizers: [0, 1],
  topRules: {"PixelProgram":[0,3]},
  specialized: [{term: 6, get: (value) => spec_Identifier[value] || -1}],
  tokenPrec: 497
})