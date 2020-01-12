$(document).ready(function() {
  var lang;
  if ($("html").attr('dir') == 'rtl') lang = 'ar'; else lang = 'en';
  
  $(".code").each(function(i, e) {
    if (!$(e).hasClass('nohighlight')) hljs.highlightBlock(e);
  });
  $(".samplecode").each(function(i, e) {
    if (!$(e).hasClass('nohighlight')) hljs.highlightBlock(e);
  });

  $(".foldable").next().hide();
  if (lang == 'ar') {
    $(".foldable").next().css('padding-right', '10px');
    $(".foldable").next().css('padding-bottom', '5px');
    $(".foldable").next().css('border-right', 'solid 1px');
    $(".foldable").next().css('border-bottom', 'solid 1px');
  } else {
    $(".foldable").next().css('padding-left', '10px');
    $(".foldable").next().css('padding-bottom', '5px');
    $(".foldable").next().css('border-left', 'solid 1px');
    $(".foldable").next().css('border-bottom', 'solid 1px');
  }
  $(".foldable").click(function() {
    if ($(this).next().is(":visible")) {
      $(this).next().slideUp("fast");
    } else {
      $(this).next().slideDown("fast");
    }
  });
});

function openAllFolds()
{
  $(".foldable").next().slideDown("fast");
}

function closeAllFolds()
{
  $(".foldable").next().slideUp("fast");
}

function sortTable(tableId, column) {
  $table = $('table#'+tableId);
  $rows = $('tr', $table);
  $rows.sort(function(a,b) {
    keyA = $('td:eq('+column+')',a).text();
    keyB = $('td:eq('+column+')',b).text();
    return (keyA > keyB) ? 1 : 0;
  });
  $rows.each(function (i, r) {
    $table.append(r);
  });
}

function addNumbering($parent, selector, prefix) {
  if (prefix == undefined) prefix = '';
  $group = $parent.children(selector);
  $group.each(function(e) {
    $(this).attr('numbering', $group.index($(this))+1);
  });
  $group.each(function(e) {
    p = prefix + $(this).attr('numbering');
    $(this).html(p + ' ' + $(this).html());
    addNumbering($(this).next(), selector, p+'.');
  });
}

