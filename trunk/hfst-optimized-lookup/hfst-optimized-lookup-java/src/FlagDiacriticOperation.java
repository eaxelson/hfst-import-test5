/**
 * A representation of one flag diacritic statement
 */

public class FlagDiacriticOperation
{
    public String operation;
    public String feature;
    public String value;
    public FlagDiacriticOperation(String op, String feat, String val)
    {
	operation = op;
	feature = feat;
	value = val;
    }
}
