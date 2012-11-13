
#!/bin/bash
TOOLDIR=../../tools/src

if [ "$4" = "--full-test" ] ; then
# Compile using xfst
    $TOOLDIR/hfst-xfst -f NumbersToNumerals.xfst.script
    rm FOO BAR
    cat NumbersToNumerals | ../xfst-att-to-$TOOLDIR/hfst-att.sh \
      > NumbersToNumerals.xfst.att

# If $1.foma.att does not exist, compile it.
if ! [ -f $1/NumbersToNumerals.foma.att ]; then
  foma -f $1/NumbersToNumerals.xfst.script 2>1 > /dev/null;
  cat $1" FOO BAR" | ../foma-att-to-$TOOLDIR/hfst-att.sh > $1/NumbersToNumerals.foma.att;
  rm $1" FOO BAR";
fi
fi

$3/$TOOLDIR/hfst-txt2fst -f openfst-tropical $1/NumbersToNumerals.xfst.att > \
  $2/NumbersToNumerals.xfst.hfst

if [ "$4" = "--full-test" ] ; then
  $3/$TOOLDIR/hfst-txt2fst -f openfst-tropical $1/NumbersToNumerals.foma.att > \
      $2/NumbersToNumerals.foma.hfst
  if ! ( $3/$TOOLDIR/hfst-compare -q $2/NumbersToNumerals.foma.hfst \
      $2/NumbersToNumerals.xfst.hfst ); then
    exit 1;
  fi
fi


$3/$TOOLDIR/hfst-project -p input $2/NumbersToNumerals.xfst.hfst > $2/expected_input
$3/$TOOLDIR/hfst-project -p output $2/NumbersToNumerals.xfst.hfst > $2/expected_output

for i in sfst openfst-tropical foma; do

  if ! ($3/$TOOLDIR/hfst-format --test-format $i); then
      continue;
  fi

  sh $1/NumbersToNumerals.hfst.script $i $3
  $3/$TOOLDIR/hfst-project -p input $2/NumbersToNumerals.hfst.hfst \
  | $3/$TOOLDIR/hfst-fst2fst -f openfst-tropical > $2/result_input
  $3/$TOOLDIR/hfst-project -p output $2/NumbersToNumerals.hfst.hfst \
  | $3/$TOOLDIR/hfst-fst2fst -f openfst-tropical > $2/result_output
  $3/$TOOLDIR/hfst-compare -q $2/expected_input $2/result_input && \
  $3/$TOOLDIR/hfst-compare -q $2/expected_input $2/result_input;
done
