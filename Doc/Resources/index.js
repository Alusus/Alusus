$(document).ready(function() {
    var lang;
    if ($("html").attr('dir') == 'rtl') lang = 'ar';
    else lang = 'en';

    $(".code").each(function(i, e) {
        if (!$(e).hasClass('nohighlight')) hljs.highlightBlock(e);
    });
    $(".samplecode").each(function(i, e) {
        if (!$(e).hasClass('nohighlight')) hljs.highlightBlock(e);
    });

    $("#changeLanguage").click(function(e) {
        var navigateToArabic = $("#changeLanguage").text().toLowerCase()[0] === "ع";

        if (navigateToArabic) {
            location.href = 'ar' + location.href.replace(location.protocol + '//' + location.host, '');
        } else {
            location.href = '..' + location.href.replace(location.protocol + '//' + location.host, '').replace('ar/', '');
        }

        e.preventDefault();
    });
});