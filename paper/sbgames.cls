%
% ''sbgames'' LaTeX class.
% created by Luiz Gonzaga da Silveira Jr - luiz@omni3.org
% UNISINOS - Last modified : jul-14-2007
% Modified by Anderson Maciel (UFRGS) - jun-05-2014
% - removed extra : after keywords 
%
% ------------ identification --------------
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{sbgames}[2007/04/05 VI SBGAMES]

% ------------ initial code --------------

\newif\ifsbc@camera
\newif\ifsbc@preprint
\newif\ifsbc@review
\newif\ifsbc@doublespaced

\newcommand{\sbc@columnmode}{}
\newcommand{\sbc@pointsize}{}

% ------------ declaration of options --------------

% 'cameraready' option.

\DeclareOption{cameraready}{%
    \sbc@cameratrue%
    \sbc@preprintfalse%
    \sbc@reviewfalse%
    \renewcommand{\sbc@columnmode}{twocolumn}%
    \sbc@doublespacedfalse%
    \renewcommand{\sbc@pointsize}{9pt}}

% 'review' option.

\DeclareOption{review}{%
    \sbc@camerafalse%
    \sbc@preprintfalse%
    \sbc@reviewtrue%
    \renewcommand{\sbc@columnmode}{twocolumn}%
    \sbc@doublespacedfalse%
    \renewcommand{\sbc@pointsize}{9pt}}

% 'widereview' option.

\DeclareOption{widereview}{%
    \sbc@camerafalse%
    \sbc@preprintfalse%
    \sbc@reviewtrue%
    \renewcommand{\sbc@columnmode}{onecolumn}%
    \sbc@doublespacedtrue%
    \renewcommand{\sbc@pointsize}{12pt}}

% 'preprint' option.

\DeclareOption{preprint}{%
    \sbc@camerafalse%
    \sbc@preprinttrue%
    \sbc@reviewfalse%
    \renewcommand{\sbc@columnmode}{twocolumn}%
    \sbc@doublespacedfalse%
    \renewcommand{\sbc@pointsize}{9pt}}

% Assume, for the moment, that we're preparing a 'cameraready' version.

\ExecuteOptions{cameraready}

% Here's a warning command for use just below.

\newcommand{\sbc@optwarning}[1]{%
    \ifsbc@camera
        \ClassWarningNoLine{sbgames}%
            {#1 option used in camera-ready mode.\MessageBreak
             This violates submission specifications}
    \fi
}

% The 'onecolumn' option doesn't work in 'cameraready' mode.

\DeclareOption{onecolumn}{%
    \sbc@optwarning{onecolumn}
    \renewcommand{\sbc@columnmode}{onecolumn}}

% The 'twocolumn' option works in 'cameraready' mode.

\DeclareOption{twocolumn}{%
    \renewcommand{\sbc@columnmode}{twocolumn}}

% Only the '9pt' size works in 'cameraready' mode.

\DeclareOption{9pt}{%
    \renewcommand{\sbc@pointsize}{9pt}}
\DeclareOption{10pt}{%
    \sbc@optwarning{10pt}
    \renewcommand{\sbc@pointsize}{10pt}}
\DeclareOption{11pt}{%
    \sbc@optwarning{11pt}
    \renewcommand{\sbc@pointsize}{11pt}}
\DeclareOption{12pt}{%
    \sbc@optwarning{12pt}
    \renewcommand{\sbc@pointsize}{12pt}}

% The 'singlespace' option works in 'cameraready' mode.

\DeclareOption{singlespace}{%
    \sbc@doublespacedfalse}

% The 'doublespace' option does not work in 'cameraready' mode.

\DeclareOption{doublespace}{%
    \sbc@optwarning{doublespace}
    \sbc@doublespacedtrue}

% No 'titlepage' option in 'cameraready' mode.

\DeclareOption{titlepage}{%
    \OptionNotUsed%
    \ClassWarningNoLine{sbgames}{titlepage option not allowed}}

% No 'landscape' mode in 'cameraready' mode, either.

\DeclareOption{landscape}{%
    \OptionNotUsed%
    \ClassWarningNoLine{sbgames}{landscape option not allowed}}

% Pass everything else to the 'artlcle' class, upon which this is based.

\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
\ProcessOptions
\PassOptionsToClass{\sbc@columnmode}{article}
\ifdim\sbc@pointsize>9pt
    \PassOptionsToClass{\sbc@pointsize}{article}
\fi

% ------------ package loading --------------

\LoadClass{article}

% ------------ main code --------------

\newcommand{\sbc@onlineid}{}
\newcommand{\onlineid}[1]{\renewcommand{\sbc@onlineid}{#1}}

\newcommand{\sbc@preprinttext}{To appear in the SBC SBGAMES conference proceedings}
\newcommand{\preprinttext}[1]{\renewcommand{\sbc@preprinttext}{#1}}

\newcommand{\sbc@reviewtext}{Online Submission ID: \sbc@onlineid}
\newcommand{\reviewtext}[1]{\renewcommand{\sbc@reviewtext}{#1}}

\newcommand{\ifcamera}[1]{\ifsbc@camera #1 \fi}
\newcommand{\ifreview}[1]{\ifsbc@review #1 \fi}
\newcommand{\ifcameraelse}[2]{\ifsbc@camera #1 \else #2 \fi}
\newcommand{\ifreviewelse}[2]{\ifsbc@review #1 \else #2 \fi}

\setlength{\textheight}{10.40in}
\setlength{\textwidth}{7.000in}
\setlength{\topmargin}{-0.700in}
\setlength{\oddsidemargin}{-0.350in}
\setlength{\evensidemargin}{-0.350in}
\setlength{\columnsep}{2pc}
\setlength{\headheight}{0.200in}
\setlength{\headsep}{0.250in}
\setlength{\footskip}{0.500in}
\setlength{\parindent}{1em}
\flushbottom

%% citation and reference definitions.

% enclose citations in square brackets.

% no labels in bibliography.

\def\@biblabel#1{}

% set the first line of each reference flush left, and each succeeding
% line with a 1em indentation.

\newlength{\bibhang}
\setlength{\bibhang}{1em}

%%%\def\thebibliography#1{\par\footnotesize
\def\thebibliography#1{%
  \section*{%
    \refname\@mkboth{\sl\uppercase{\refname}}{\sl\uppercase{\refname}}}
  \list{\relax}{\setlength{\labelsep}{0em}
                \setlength{\itemindent}{-\bibhang}
                \setlength{\leftmargin}{\bibhang}}
  \def\newblock{\hskip .11em plus .33em minus .07em}
  \sloppy\clubpenalty4000\widowpenalty4000
  \sfcode`\.=1000\relax}

\def\@citex[#1]#2{\if@filesw\immediate\write\@auxout{\string\citation{#2}}\fi
  \def\@citea{}\@cite{\@for\@citeb:=#2\do
    {\@citea\def\@citea{; }\@ifundefined
      {b@\@citeb}{{\bf ?}\@warning
      {Citation '\@citeb' on page \thepage \space undefined}}%
{\csname b@\@citeb\endcsname}}}{#1}}

\let\@internalcite\cite
\def\cite{\def\citename##1{##1}\@internalcite}
\def\shortcite{\def\citename##1{}\@internalcite}

%%

\renewcommand{\ps@plain}%
    {%
    \renewcommand{\@oddhead}{}%
    \renewcommand{\@oddfoot}{}%

    \ifsbc@preprint%
      \renewcommand{\@oddhead}{\hfil\textit{\large\sbc@preprinttext}\hfil}%
      \renewcommand{\@oddfoot}{\hfil\textrm{\thepage}\hfil}%
    \fi%

    \ifsbc@review%
      \renewcommand{\@oddhead}{\hfil\textit{\large\sbc@reviewtext}\hfil}%
      \renewcommand{\@oddfoot}{\hfil\textrm{\thepage}\hfil}%
    \fi%

    \renewcommand{\@evenhead}{\@oddhead}%
    \renewcommand{\@evenfoot}{\@oddfoot}%
    }

\renewcommand{\ps@empty}%
    {%
    \renewcommand{\@oddhead}{}%
    \renewcommand{\@oddfoot}{}%

    \ifsbc@preprint%
      \renewcommand{\@oddhead}{\hfil\textit{\large\sbc@preprinttext}\hfil}%
      \renewcommand{\@oddfoot}{\hfil\textrm{\thepage}\hfil}%
    \fi%

    \ifsbc@review%
      \renewcommand{\@oddhead}{\hfil\textit{\large\sbc@reviewtext}\hfil}%
      \renewcommand{\@oddfoot}{\hfil\textrm{\thepage}\hfil}%
    \fi%

    \renewcommand{\@evenhead}{\@oddhead}%
    \renewcommand{\@evenfoot}{\@oddfoot}%
    }

% no page numbers - they are added in production

\pagestyle{plain}

\newcommand{\sbc@setninepoint}{
    \renewcommand\normalsize{%
        \@setfontsize\normalsize\@ixpt\@xpt
        \abovedisplayskip 9\p@ \@plus2\p@ \@minus4\p@
        \abovedisplayshortskip \z@ \@plus3\p@
        \belowdisplayshortskip 6\p@ \@plus3\p@ \@minus3\p@
        \belowdisplayskip \abovedisplayskip
        \let\@listi\@listI}
    \renewcommand\small{%
        \@setfontsize\small\@viipt\@ixpt
        \abovedisplayskip 8.5\p@ \@plus3\p@ \@minus4\p@
        \abovedisplayshortskip \z@ \@plus2\p@
        \belowdisplayshortskip 4\p@ \@plus2\p@ \@minus2\p@
        \def\@listi{\leftmargin\leftmargini
                    \topsep 4\p@ \@plus2\p@ \@minus2\p@
                    \parsep 2\p@ \@plus\p@ \@minus\p@
                    \itemsep \parsep}%
        \belowdisplayskip \abovedisplayskip}
    \renewcommand\footnotesize{%
        \@setfontsize\footnotesize\@viiipt{9.5}%
        \abovedisplayskip 6\p@ \@plus2\p@ \@minus4\p@
        \abovedisplayshortskip \z@ \@plus\p@
        \belowdisplayshortskip 3\p@ \@plus\p@ \@minus2\p@
        \def\@listi{\leftmargin\leftmargini
                    \topsep 3\p@ \@plus\p@ \@minus\p@
                    \parsep 2\p@ \@plus\p@ \@minus\p@
                    \itemsep \parsep}%
        \belowdisplayskip \abovedisplayskip}
    \renewcommand\scriptsize{\@setfontsize\scriptsize\@viipt\@viiipt}
    \renewcommand\tiny{\@setfontsize\tiny\@vpt\@vipt}
    \renewcommand\large{\@setfontsize\large\@xpt\@xiipt}
    \renewcommand\Large{\@setfontsize\Large\@xiipt{14}}
    \renewcommand\LARGE{\@setfontsize\LARGE\@xivpt{18}}
    \renewcommand\huge{\@setfontsize\huge\@xviipt{22}}
    \renewcommand\Huge{\@setfontsize\Huge\@xxpt{25}}
    \selectfont
}
\ifdim\sbc@pointsize=9pt
    \sbc@setninepoint
\fi

\newcommand{\sbc@sectionfont}{}
\newcommand{\sectionfont}[1]{\renewcommand{\sbc@sectionfont}{#1}}
\renewcommand\section{\@startsection {section}{1}{\z@}%
                {-1.0ex}%
                {1.0ex}%
                {\reset@font\Large\sffamily\bfseries\sbc@sectionfont}}
\renewcommand\subsection{\@startsection{subsection}{2}{\z@}%
                {-1.0ex}%
                {1.0ex}%
                {\reset@font\large\sffamily\bfseries\sbc@sectionfont}}
\renewcommand\subsubsection{\@startsection{subsubsection}{3}{\z@}%
                {-1.0ex}%
                {1.0ex}%
                {\reset@font\normalsize\sffamily\bfseries\sbc@sectionfont}}

\renewcommand\paragraph{\@startsection{paragraph}{4}{\z@}%
                {2.05ex}%
                {-1em}%
                {\reset@font\normalsize\sffamily\bfseries\sbc@sectionfont}}
\renewcommand\subparagraph{\@startsection{subparagraph}{5}{\parindent}%
                {3.25ex \@plus1ex \@minus .2ex}%
                {-1em}%
                {\reset@font\normalsize\sffamily\bfseries\sbc@sectionfont}}

\newcommand{\sbc@empty}{}

\newcommand{\sbc@affiliation}{}
\newcommand{\affiliation}[1]{\renewcommand{\sbc@affiliation}{#1}}

\newcommand{\sbc@format}{}
\newcommand{\format}[1]{\renewcommand{\sbc@format}{#1}}
\newcommand{\sbcformat}[1]{\format{#1}}

\newcommand{\sbc@teaser}{}
\newcommand{\teaser}[1]{\renewcommand{\sbc@teaser}{#1}}

\newlength{\titlespace}
\setlength{\titlespace}{0.25in}

\newlength{\teaserspace}
\setlength{\teaserspace}{0.25in}

\ifsbc@review
    \renewcommand{\@maketitle}{%
        \begin{center}%
            \renewcommand{\thanks}[1]{}
            {\LARGE\sffamily\bfseries\sbc@sectionfont \@title \par}%
            \vspace{1\baselineskip}%
            \vspace{\titlespace}%
            \ifx\sbc@teaser\sbc@empty \else%
                \begingroup%
                    \def\@captype{figure}%
                    \sbc@teaser%
                \endgroup\par%
                \vspace{\teaserspace}%
            \fi%
        \end{center} \par%
    }
\else
    \renewcommand{\@maketitle}{%
        \begin{center}%
            {\LARGE\sffamily\bfseries\sbc@sectionfont \@title \par}%
            \vspace{1\baselineskip}%
            \large
            \begin{tabular}[t]{c}%
                \@author
            \end{tabular}\par%
            \ifx\sbc@affiliation\sbc@empty \else%
                \vspace{0.5\baselineskip}%
                \sbc@affiliation\par%
            \fi%
            \vspace{\titlespace}%
            \ifx\sbc@teaser\sbc@empty \else%
                \begingroup%
                    \def\@captype{figure}%
                    \sbc@teaser%
                \endgroup\par%
                \vspace{\teaserspace}%
            \fi%
        \end{center} \par%
    }
\fi

\let\sbc@origmaketitle\maketitle
\let\sbc@origand\and
\renewcommand{\maketitle}{%
    \let\sbc@title\@title%
    \let\sbc@author\@author%
    \sbc@origmaketitle%
    \thispagestyle{empty}%
    \ifsbc@doublespaced%
        \renewcommand{\baselinestretch}{1.66}\selectfont%
    \fi
}

\newtoks\sbc@abs
\ifsbc@review
    \long\def\sbc@add#1{\global\sbc@abs\expandafter{\the\sbc@abs#1}}
    \long\def\sbc@collect{%
        \global\sbc@abs{}%
        \let\abstract\sbc@@collect
        \abstract
    }
    \long\def\sbc@@collect#1\end#2{%
        \def\@tempa{#2}%
        \ifx\@tempa\@currenvir
            \sbc@add{#1}%
            \edef\abstract{\noexpand\end{\@tempa}}%
        \else
            \sbc@add{#1\end{#2}}%
        \fi
        \abstract
    }
    \newcommand{\sbc@modify}{%
        \let\sbc@origabs\abstract%
        \let\sbc@origendabs\endabstract%
        \renewenvironment{abstract}%
            {\sbc@collect}%
            {\begingroup
                \let\abstract\sbc@origabs
                \let\endabstract\sbc@origendabs
                \begin{abstract} \the\sbc@abs \end{abstract}
            \endgroup}%
    }
    \AtBeginDocument{\sbc@modify}
\fi
\newcommand{\keywordlist}{%
    \vspace{0.5\baselineskip}%
    \par\noindent \textbf{Keywords: } \sbc@keywords%
}

\newcommand{\contactlist}{%
    \vspace{0.5\baselineskip}%
    \par\noindent \textbf{Author's Contact: } \\\par\indent 
    \begin{tabular}[t]{l}%
      \sbc@contactinfo
    \end{tabular}\par%
    
}

\newenvironment{CRcatlist}{%
    \vspace{0.5\baselineskip}%
    \par\noindent \textbf{CR Categories: }%
}{}

\newcommand{\CRcat}[4]{%
    #1 [#2]%
    \ifx#3\sbc@empty \else : #3\fi%
    \ifx#4\sbc@empty \else ---#4\fi%
}

% leave a X inch space (default X=0.0) at the bottom of the left column
% on the first page for the copyright block.

\newlength{\sbc@copyrightlength}
\setlength{\sbc@copyrightlength}{0.0in}

\newcommand{\copyrightspace}{%
    \renewcommand{\thefootnote}{}%
    %\footnotetext[0]{\rule[\sbc@copyrightlength]{2.71828in}{0in}}%
    \renewcommand{\thefootnote}{\arabic{footnote}}
}

%\renewcommand{\footnoterule}{%
%    \kern-3pt
%    \hrule width 0.45\columnwidth
%    \kern 2.6pt}

\newcommand{\sbc@contactinfo}{}
\newcommand{\contactinfo}[1]{\renewcommand{\sbc@contactinfo}{#1}}

\newcommand{\sbc@contactaddress}{}
\newcommand{\contactaddress}[1]{\renewcommand{\sbc@contactaddress}{#1}}

\newcommand{\sbc@contactphone}{}
\newcommand{\contactphone}[1]{\renewcommand{\sbc@contactphone}{#1}}

\newcommand{\sbc@contactfax}{}
\newcommand{\contactfax}[1]{\renewcommand{\sbc@contactfax}{#1}}

\newcommand{\sbc@contactemail}{}
\newcommand{\contactemail}[1]{\renewcommand{\sbc@contactemail}{#1}}

\newcommand{\sbc@keywords}{}
\newcommand{\keywords}[1]{\renewcommand{\sbc@keywords}{#1}}

\newcommand{\sbc@estpages}{}
\newcommand{\estpages}[1]{\renewcommand{\sbc@estpages}{#1}}

\newif\ifsbc@cover
\sbc@coverfalse
%\ifsbc@review
%    \sbc@covertrue
%\else
%    \sbc@coverfalse
%\fi

\newcommand{\suppresscover}{\sbc@coverfalse}
\newcommand{\sbc@coverpage}{%
    \begin{titlepage}%
        \def\thanks##1{}
        \let\and\sbc@origand
        \sbc@setninepoint\normalsize
        \renewcommand{\baselinestretch}{1}\selectfont
        \begin{center}%
            \vspace*{\fill}
            {\LARGE\sffamily\bfseries\sbc@sectionfont \sbc@title \par}%
            \vspace{2\baselineskip}%
            {\large
                \begin{tabular}[t]{c}%
                \sbc@author
             \end{tabular}\par%
            }%
            \vspace{1\baselineskip}%
            {\large \sbc@affiliation \par}%
            \addvspace{3\baselineskip}%
            \vspace{0.5\baselineskip}%
            {Format: \sbc@format \par}%
            \vspace{3\baselineskip}%
            \begin{tabular}{ll}
                Contact: & \sbc@contactinfo \\[1\baselineskip]
                         & \begin{tabular}[b]{@{}l@{}}
                               \sbc@contactaddress
                               \end{tabular} \\[1\baselineskip]
                phone: & \sbc@contactphone \\
                fax: & \sbc@contactfax \\
                email: & \sbc@contactemail
            \end{tabular}\par%
            \vspace*{3\baselineskip}%
            {Estimated \# of pages: \sbc@estpages \par}%
            \vspace*{\baselineskip}%
            {Keywords: \sbc@keywords \par}%
            {\textbf{Author's Contact: } \sbc@contactinfo \par}%
            \vspace*{\fill}%
            \begin{minipage}{5in}%
                \the\sbc@abs
            \end{minipage}\par%
            \vspace*{\fill}
        \end{center}%
    \end{titlepage}%
}

\AtEndDocument{\ifsbc@cover \sbc@coverpage \fi}
\newcommand{\sbcopening}[1]{%
  \ClassError{sbgames}{%
    The sbcopening command is no longer needed.\MessageBreak%
    Switch to the maketitle command and abstract environment}{}}
\InputIfFileExists{sbgames.cfg}
        {\typeout{***************************************^^J%
                  * Local config file sbgames.cfg used *^^J%
                  ***************************************}}
        {}
\endinput

% End of file 'sbgames.cls'.

