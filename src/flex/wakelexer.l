%{
#include <stdio.h>
#include "tree.h"
#include "wake.tab.h"

extern YYSTYPE wakelval;
extern int line;
extern int column;

void count();
#include "parseUtil.h"

%}

%x ST_BLOCKCOMMENT
%x ST_LINECOMMENT

%%

#>						count(); BEGIN(ST_BLOCKCOMMENT);
##						count();
<ST_BLOCKCOMMENT>##		count(); BEGIN(INITIAL);
<ST_BLOCKCOMMENT>.		count();
<ST_BLOCKCOMMENT>\n		line++; column = 1;

<INITIAL>\/\/			count(); BEGIN(ST_LINECOMMENT);
<ST_LINECOMMENT>\n		line++; column = 1; BEGIN(INITIAL);
<ST_LINECOMMENT>.		count();

#[a-zA-Z0-9]+			count();
`[^`]*`					count();

every					count(); return EVERY;
capable					count(); return CAPABLE;
an?						count(); return A_OR_AN;
is						count(); return IS;
return					count(); return RETURN;
foreach					count(); return FOREACH;
with					count(); return WITH;
public					count(); return PUBLIC;
private					count(); return PRIVATE;
if						count(); return IF;
else					count(); return ELSE;
while					count(); return WHILE;
in						count(); return IN;
import					count(); return IMPORT;
provides				count(); return PROVIDES;
needs					count(); return NEEDS;
then					count(); return THEN;
case					count(); return CASE;
default					count(); return DEFAULT;
switch					count(); return SWITCH;
for						count(); return FOR;
break					count(); return BREAK;
do						count(); return DO;
continue				count(); return CONTINUE;
this					count(); return THIS;
parent					count(); return PARENT;
fn						count(); return FN;
nothing					count(); return NOTHING;
cast					count(); return CAST;
exists					count(); return EXISTS;
var						count(); return VAR;
foreach					count(); return FOREACH;
in						count(); return IN;
from					count(); return FROM;

\<\(					count(); return SYM_PROVIDE_ARGS_OPEN;
\+\+					count(); return SYM_INCREMENT;
!=						count(); return SYM_NE;
==						count(); return SYM_EQ;
&&						count(); return SYM_AND;
\|\|					count(); return SYM_OR;
\<-						count(); return SYM_PROVIDE;
\<=						count(); return SYM_LE;
\>=						count(); return SYM_GE;
:=						count(); return SYM_VALEQ;
\+=						count(); return SYM_PLUSEQ;
-=						count(); return SYM_SUBEQ;
\/=						count(); return SYM_DIVEQ;
\*=						count(); return SYM_MULTEQ;
--						count(); return SYM_RETURN_DECREMENT;
\?\?\?					count(); return SYM_CURRIER;
:						count(); return ':';
@						count(); return '@';
\(						count(); return '(';
\)						count(); return ')';
\{						count(); return '{';
\}						count(); return '}';
\[						count(); return '[';
\]						count(); return ']';
;						count(); return ';';
,						count(); return ',';
\.						count(); return '.';
-						count(); return '-';
=						count(); return '=';
\/						count(); return '/';
\\						count(); return '\\';
#						count(); return '#';
\|						count(); return '|';
\*						count(); return '*';
!						count(); return '!';
\?						count(); return '?';
\+						count(); return '+';
\<						count(); return '<';
\>						count(); return '>';

[\t\r ]					count();
\n						line++; column = 1;

\$+						count(); wakelval.number = strlen(waketext); return SYM_SHADOW;
(\[\])+					count(); wakelval.number = strlen(waketext)>>1; return SYM_ARRAYED;

true					count(); wakelval.number = 1; return BOOL;
false					count(); wakelval.number = 0; return BOOL;
-?[0-9]+(\.[0-9]+)?		count(); wakelval.number = atof(waketext); return NUMBER;
:[a-zA-Z0-9]+			count(); wakelval.string = strdup(waketext+1); return SPECIALTY;
[a-z][a-zA-Z0-9]*		count(); wakelval.string = strdup(waketext); return LIDENTIFIER;
[A-Z][a-zA-Z0-9]*		count(); wakelval.string = strdup(waketext); return UIDENTIFIER;
\~\[[^\]]+\]\~			count(); wakelval.string = strdup(waketext); return LIDENTIFIER;
\"(\\.|[^\\"])*\"		count(); wakelval.string = unEscapeStringLiteral(keepInner(waketext), '"'); return(STRING);
'(\\.|[^\'])*\'			count(); wakelval.string = unEscapeStringLiteral(keepInner(waketext), '\''); return(STRING);

.						count(); return ERRORTOKEN;
%%

void count()
{
	int i;

	for (i = 0; waketext[i] != '\0'; i++)
	switch(waketext[i]) {
		case '\t':
			column += 4 - ((column + 1) % 4); break;
		case '\n':
			column = 1; line++; break;
		default:
			column++;
	}
}
