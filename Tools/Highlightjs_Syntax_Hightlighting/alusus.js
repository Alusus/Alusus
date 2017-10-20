/*
Language: Alusus
Author: Sarmad Khalid Abdullah <sarmad@alusus.org>
*/

function(hljs) {
  var KEYWORDS =
      'trule prule prule_group module type integer string list map lexer start as ref in ' +
      'prefix inherits self root dimension import def function output use cast cnt ptr array any ' +
      'null false int float while private char catch export virtual operator sizeof ' +
      'dynamic_cast typedef const_cast|10 const struct for static_cast union namespace ' +
      'unsigned long throw volatile static protected bool template mutable if public friend ' +
      'do return goto auto void enum else break new extern using true class case typeid ' +
      'short reinterpret_cast default double register explicit signed typename try this ' +
      'switch continue inline delete alignof finally ' +
      'noexcept nullptr static_assert thread_local restrict alias run build link ' +
      'اشمل عرّف حزمة دالّة هيكل إذا بينما لكل أرجع مؤشر محتوى مصفوفة أي عدد_حقيقي عدد_حقيقي64 سلسلة_محارف لقب نفّذ ';
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
        begin: '\\b(\\d+(\\.\\d*)?|\\.\\d+)(u|U|l|L|ul|UL|f|F)'
      },
      {
        className: 'attribute',
        begin: '@<?[a-zA-Z_][a-zA-Z_0-9]+'
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
        begin: '\\b[A-Z][a-zA-Z_0-9]+'
      },
      {
        className: 'type',
        begin: '[\u0620-\u065F\u066E-\u06DC_0-9]+_ال[\u0620-\u065F\u066E-\u06DC_0-9]+'
      },
      {	
        className: 'type',
        begin: '[^\u0620-\u065F\u066E-\u06DC_0-9]ال[\u0620-\u065F\u066E-\u06DC_0-9]+'
      }
    ]
  };
}
