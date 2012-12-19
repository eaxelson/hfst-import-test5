#!/bin/bash
PATH=${PWD}/bin:${PATH}
# Maximum length for input data
MAXLENGTH=600
# Parse parameters and build title
TITLE="HFST web demo"
HEADER="HFST"
if test ! -z $QUERY_STRING ; then
    PARAMS=$(echo $QUERY_STRING | tr '?;&' ' ')
    for p in $PARAMS ; do
        case $p in
            language=*)
                LL=$(echo $p | sed -e 's/language=//');;
            wf=*)
                WORDFORM=$(echo $p | sed -e 's/wf=//' | sed 's/\\/\\\\/g;s/%\([0-9a-fA-F][0-9a-fA-F]\)/\\\\x\1/g' | xargs echo -e) ;;
            function=*)
                FUNCTION=$(echo $p | sed -e 's/function=//' | tr -c -d '[:alnum:]-_');;
            variant=*)
                VARIANT=$(echo $p | sed -e 's/variant=//' | tr -c -d '[:alnum:]-_');;
            *)
                echo "Content-Type: text/plain";
                echo;
                echo "Failed to parse $QUERY_STRING";
                echo "possibly broken near $p";
                exit 1;;
        esac
    done
    # base title on provided *.description files if possible
    if test -r hfst/$LL/language.description ; then
        LANGNAME="$(cat hfst/$LL/language.description)"
    elif test -r hfst/$LL.description ; then
        LANGNAME="$(cat hfst/$LL.description)"
    else
        LANGNAME="language code: $LL"
    fi
    if test -r hfst/$LL/$FUNCTION.description ; then
        TYPEDESC="$(cat hfst/$LL/$FUNCTION.description)"
    elif test -r hfst/$FUNCTION.description ; then
        TYPEDESC="$(cat hfst/$FUNCTION.description)"
    else
        TYPEDESC="functionality: $FUNCTION"
    fi
    if test -r hfst/$LL/$VARIANT.description ; then
        NAMEDESC="$(cat hfst/$LL/$VARIANT.description)"
    elif test -r hfst/$VARIANT.description ; then
        NAMEDESC="$(cat hfst/$VARIANT.description)"
    else
        NAMEDESC="version: $VARIANT"
    fi
    TITLE="$TITLE :: $LL :: $FUNCTION :: $VARIANT"
    HEADER="$HEADER, $LANGNAME, $TYPEDESC, $NAMEDESC"
fi

# HTTP headers
function print_http_headers() {
    echo "Content-Type: text/html; charset=UTF-8"
    echo
}

# the HTML5 head and form handling scripts
function print_html_head() {
    cat <<EOHEAD
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title>$TITLE</title>
    <script type="text/javascript" language="javascript">
function clear_options(options)
  {
      for (i = options.length; i >= 0; i--)
      {
        options[i] = null
      }
  }

function setSelectedIndex(s, v) {
    alert("setting selected index")
    for ( var i = 0; i < s.options.length; i++ ) {
        if ( s.options[i].value == v ) {
            s.options[i].selected = true;
            return;
        }
    }
}
function add_language(code, text) 
  {
    var opt = document.createElement("option")
    document.getElementById("languages").options.add(opt)
    opt.text = text
    opt.value = code
  }

function add_function(name, text) 
  {
    var opt = document.createElement("option")
    document.getElementById("functions").options.add(opt)
    opt.text = text
    opt.value = name
  }

function add_variant(name, text) 
  {
    var opt = document.createElement("option")
    document.getElementById("variants").options.add(opt)
    opt.text = text
    opt.value = name
  }

function populate_languages() 
  {
    if (!document.getElementById("languages"))
        {
            return true
        }
    clear_options(document.getElementById("languages"));
EOHEAD
    # all languages should have javascript for adding language variants to 
    # selector
    for ll in hfst/*/ ; do
        if test -r $ll/languages.js ; then
            cat $ll/languages.js | sed -e 's/^/    /'
        fi
    done
    echo "    populate_functions()"
    echo "}"
    echo
    echo "function populate_functions() {"
    echo "  clear_options(document.getElementById('functions'))"
    echo "  var languages = document.getElementById('languages')"
    # all languages should have javascript for adding supported functions
    # to list
    for ll in hfst/*/ ; do
        LANGCODE=$(echo $ll | sed -e 's/^hfst\///' -e 's/\/$//')
        if test -r $ll/functions.js ; then
            echo "  if (languages[languages.selectedIndex].value == '$LANGCODE') {"
            if test -r $ll/placeholder.string ; then
                echo "document.getElementById('wf').placeholder = '$(cat $ll/placeholder.string)'"
            fi
            cat $ll/functions.js | sed -e 's/^/    /'
            echo "  }"
        fi
    done
    echo "  populate_variants()"
    echo "}"
    echo
    echo "function populate_variants() {"
    echo "  clear_options(document.getElementById('variants'))"
    echo "  var languages = document.getElementById('languages')"
    echo "  var funcs = document.getElementById('functions')"
    # all languages
    for ll in hfst/*/ ; do
        LANGCODE=$(echo $ll | sed -e 's/^hfst\///' -e 's/\/$//')
        echo "  if (languages[languages.selectedIndex].value == '$LANGCODE') {"
        # and each functions must have javascript for adding supported
        # variants for supported functions to lsit
        for ff in $(cat $ll/functions) ; do
            if test -r $ll/$ff.variants.js ; then
                echo "    if (funcs[funcs.selectedIndex].value == '$ff') {"
                cat $ll/$ff.variants.js | sed -e 's/^/      /'
                echo "    }"
            fi
        done
        echo "  }"
    done
    echo "}"
    echo "function init_forms() {"
    echo "  populate_languages()"
    if ! test -z $LL ; then
        echo "  langs = document.getElementById('languages')"
        echo "  for (var i = 0; i < langs.options.length; i++ ) {"
        echo "    if (langs.options[i].value == '$LL' ) {"
        echo "      langs.options[i].selected = true"
        echo "    }"
        echo "  }"
        echo "  populate_functions()"
    fi
    if ! test -z $FUNCNAME ; then
        echo "  funcs = document.getElementById('functions')"
        echo "  for (var i = 0; i < funcs.options.length; i++ ) {"
        echo "    if (funcs.options[i].value == '$FUNCTION' ) {"
        echo "      funcs.options[i].selected = true"
        echo "    }"
        echo "  }"
        echo "  populate_variants()"
    fi
    if ! test -z $VARIANT ; then
        echo "  varis = document.getElementById('variants')"
        echo "  for (var i = 0; i < varis.options.length; i++ ) {"
        echo "    if (varis.options[i].value == '$VARIANT' ) {"
        echo "      varis.options[i].selected = true"
        echo "    }"
        echo "  }"
    fi
    echo "  document.getElementById('wf').focus()"
    echo "}"
    echo "</script>"
    # site may have additional html > head children
    if test -r head.html ; then
        cat head.html
    fi
    # languages may have additional html > head children
    if test -r hfst/$LL/head.html ; then
        cat hfst/$LL/head.html
    fi
    echo "</head>"
    echo '<body onload="init_forms()">'
}

function print_html_heading_info() {
    # the domain may have additional heading
    if test -r heading.html ; then
        cat heading.html
    fi
    cat<<EOHEADING
  <h1>$HEADER</h1>
  <p style="font-size: smaller;">
    These demos are based on the <a href="http://hfst.sf.net/">HFST</a>
    finite-state versions of language models. All software and language
    models can be downloaded from HFST download pages.
  </p>
EOHEADING
    # languages may have additional data for headings
    if test -r hfst/$LL/heading.html ; then
        cat hfst/$LL/heading.html
    fi
    # specific selected functions may have additional data for headings
    if test -r hfst/$LL/heading.$FUNCTION.$VARIANT.html ; then
        cat hfst/$LL/heading.$FUNCTION.$VARIANT.html
    fi
    if test -r hfst/$LL/placeholder.string ; then
        echo "<p style=\"font-style: smaller;\">
            If you do not understand the langauge you are experimenting
            with and are unable to obtain example word from <a 
             href='http://$LL.wikipedia.org/'>$LL wikipedia</a>, you may
            use <tt>"
        cat hfst/$LL/placeholder.string
        echo "</tt> as a test case</p>"
    fi
}

function print_html_footer() {
    # the specific functions may have additional data for footers
    if test -r hfst/$LL/footer.$FUNCTION.$VARIANT.html ; then
        cat hfst/$LL/footer.$FUNCTION.$VARIANT.html
    fi
    # the languages may have additional data for footers
    if test -r hfst/$LL/footer.html ; then
        cat hfst/$LL/footer.html
    fi
    cat<<EOFOOT
  <hr title="licence and metadata section begins">
  <div style="font-size:smaller">
    This demo is &copy; University of Helsinki 2010 under 
    <a href="http://www.gnu.org/licenses/agpl.html">
        <img title="GNU Affero General Public Licence version 3" alt="AGPLv3"
         src="data:image/png;base64,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
WZ3Yafuc2/8ArY/Dax67qJ0AAAAASUVORK5CYII="/>
    </a>,
    <a href="http://hfst.svn.sourceforge.net/viewvc/hfst/web-demo-bash/$(basename $SCRIPT_NAME)">
      the demo source code is available in HFST svn
    </a>.
EOFOOT
    if test -r hfst/$LL/footer-licence.html ; then
        cat hfst/$LL/footer-licence.html
    fi
    cat << EOEND
  </div>
</body>
</html>
EOEND
}

function print_forms() {
    if test -r hfst/$LL/placeholder.string ; then
        PLACEHOLDER=$(cat hfst/$LL/placeholder.string)
    else
        PLACEHOLDER="insert wordform"
    fi
    cat <<FORMS
    <form action='$SCRIPT_NAME' method='GET' accept-charset='UTF-8'>
      <fieldset>
      <legend>Process single word form</legend>
      <label>Space separated word forms to process (max. $MAXLENGTH bytes):
        <textarea maxlength="$MAXLENGTH" name="wf" id="wf" required
         placeholder="$PLACEHOLDER">${WORDFORM//+/ }</textarea></label>
      <!-- these defaults are left in if javascript fails -->
      <label>Language <select name="language" id="languages" onchange="populate_functions()">
        <option value="">Select to get list of options:</option>
        <option value="fi">Finnish</option>
      </select></label>
      <select name="function" id="functions" onchange="populate_variants()">
        <option value="">Select function*</option>
        <option value="morphology">Morphological analysis</option>
        <option value="error">Spell checking</option>
        <option value="generate">Word form generation</option>
        <option value="guess">Word inflection guessing</option>
        <option value="hyphenate">Hyphenate word forms</option>
      </select></label>
      <select name="variant" id="variants">
        <option value="">Select variant*</option>
        <option value="omor">Omor</option>
        <option value="cg">cg</option>
        <option value="hunspell">Hunspell</option>
        <option value="edit-distance-2">Edit Distance of 2</option>
      </select>
      <div>* available functions and variants depend on language selection</div>
      <input type="submit" value="Process wordform"/>
      </fieldset>
    </form>
FORMS
}


function print_results() {
    if test -x hfst/$LL/$FUNCTION.$VARIANT.bash ; then
        hfst/$LL/morphology.$VARIANT.bash $WORDFORM 2>&1
    elif test -x hfst/$LL/$FUNCTION.bash ; then
        hfst/$LL/$FUNCTION.bash $WORDFORM $VARIANT 2>&1
    elif test -x hfst/$FUNCTION.$VARIANT.bash ; then
        hfst/$FUNCTION.$VARIANT.bash $WORDFORM $LL 2>&1
    elif test -x hfst/$FUNCTION.bash ; then
        hfst/$FUNCTION.bash $WORDFORM $LL $VARIANT 2>&1
    elif test -r hfst/$LL/$FUNCTION.$VARIANT.hfstol ; then
        echo "<pre>"
        echo $WORDFORM | hfst-lookup hfst/$LL/$FUNCTION.$VARIANT.hfstol
        echo "</pre>"
    elif test -r hfst/$LL/$FUNCTION.$VARIANT.hfst ; then
        echo "<pre>"
        echo $WORDFORM | hfst-lookup hfst/$LL/$FUNCTION.$VARIANT.hfst
        echo "</pre>"
    fi
}



print_http_headers
print_html_head
print_html_heading_info
if ! test -z $QUERY_STRING ; then
    print_results
fi
print_forms
print_html_footer
exit 0

