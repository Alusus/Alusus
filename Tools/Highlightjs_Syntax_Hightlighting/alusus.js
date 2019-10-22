/*
Language: Alusus
Author: Sarmad Khalid Abdullah <sarmad@alusus.org>
*/

function(hljs) {
  var KEYWORDS =
    'trule prule prule_group module type macro integer string list map lexer start as ref in ' +
    'prefix inherits self root dimension import def function output use cast cnt ptr array any ' +
    'null false while private catch export virtual operator sizeof ' +
    'dynamic_cast typedef const_cast|10 const struct for static_cast union namespace ' +
    'unsigned long throw volatile static protected template mutable if public friend ' +
    'do return goto auto enum else break new extern using true class case typeid ' +
    'short reinterpret_cast default double register explicit signed typename try this ' +
    'switch continue inline delete alignof finally and or not on this this_type value ' +
    'noexcept nullptr static_assert thread_local restrict alias dump_ast init ' +
    'اشمل عرف عرّف وحدة دالة دالّة هيكل إذا بينما لكل أرجع ارجع مؤشر محتوى مصفوفة أي  لقب ' +
    'صنف وإلا اقطع أكمل أدرج_ش_ب_م و أو ليس ماكرو استخدم سند عند هذا هذا_الصنف قيمة هيئ';
  return {
    aliases: ['alusus'],
    keywords: KEYWORDS,
    lexemes: /[A-Za-z0-9_\u0620-\u065F\u066E-\u06DC]+/,
    contains: [
      hljs.C_LINE_COMMENT_MODE,
      hljs.C_BLOCK_COMMENT_MODE,
      hljs.QUOTE_STRING_MODE,
      {
        className: 'string',
        begin: '\'\\\\?.', end: '\'',
        illegal: '.'
      },
      {
        className: 'number',
        begin: '\\b(\\d+(\\.\\d*)?|\\.\\d+)(u|U|i|I|ui|UI|f|F|ط|ص|طص|ع)?[0-9]*'
      },
      {
        className: 'attribute',
        begin: '@<?[a-zA-Z_][a-zA-Z_0-9]*'
      },
      {
        className: 'attribute',
        begin: '@<?[\u0620-\u065F_][\u0620-\u065F_0-9]*'
      },
      hljs.C_NUMBER_MODE,
      {
        className: 'preprocessor',
        begin: '#', end: '$',
        contains: [
          {begin: 'include\\s*<', end: '>', illegal: '\\n'},
          hljs.C_LINE_COMMENT_MODE
        ]
      },
      {
        className: 'keyword',
        begin: '~[A-Za-z0-9_\u0620-\u065F\u066E-\u06DC]+'
      },
      {
        className: 'type',
        begin: '(^|\\W)[A-Z][a-zA-Z_0-9]+'
      },
      {
        className: 'type',
        begin: '(^|\\W)[اأإورزدذ]*[\u0620-\u065F]ـ[\u0620-\u065F_0-9]*'
      },
      {
        className: 'type',
        begin: '(^|[^\u0620-\u065F_0-9A-Za-z])(int|word|float|bool|char|void|صحيح|طبيعي|عائم|ثنائي|محرف|فراغ)($|[^\u0620-\u065F_0-9A-Za-z])'
      },
      {
        className: 'string',
        begin: '(^|\\W)[A-Z]+'
      },
      {
        className: 'string',
        begin: '(^|[^\u0620-\u065F_0-9])_[\u0620-\u065F_0-9]+_($|[^\u0620-\u065F_0-9])'
      },
    ]
  };
}
