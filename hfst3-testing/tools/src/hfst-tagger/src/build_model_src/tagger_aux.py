import math
import collections
import string

class InvalidPattern(Exception):
    def __init__(self):
        super(InvalidPattern, self).__init__()

class ReachesSequenceEnd(Exception):
    def __init__(self):
        super(ReachesSequenceEnd, self).__init__()

class SequenceSimplifier:

    def __init__(self,pattern):

        for line in pattern:
            
            if len(line) != 2 or (not line[0] in [0,1]):
                
                raise InvalidPattern()

        self.pattern = pattern
            
    def simplify(self,i,sequence):

        if i + len(self.pattern) >= len(sequence):
            
            raise ReachesSequenceEnd()

        simplified_subsequence = []

        for pair in self.pattern:
            
            simplified_entry = ["<NONE>","<NONE>"]
            
            if pair[0] == 1:
                simplified_entry[0] = sequence[i][0]

            if pair[1] == 1:
                simplified_entry[1] = sequence[i][1]

            simplified_subsequence.append(tuple(simplified_entry))

            i += 1

        return tuple(simplified_subsequence)

# Unless line is a utf-8 encoded line of number_of_fields tab
# separated fields, raise an exception.
def check_line(line,number_of_fields):
    try:
        line.decode("utf-8")
    except UnicodeDecodeError:
        raise Exception("Invalid utf-8 encoding on line")

    if len(line.split("\t")) != number_of_fields:
        raise Exception("Wrong number of fields on line")

# Return tropical penalty corresponding to the probability
# suffix_and_tag_count/suffix_count.
def get_penalty(suffix_and_tag_count, suffix_count):

    return math.log(suffix_count) - math.log(suffix_and_tag_count)


# Return the utf-8 string str reversed. utf-8 symbols are not
# internally reversed.
def reverse(str):

    u_str = unicode(str, "utf-8")

    u_str_array = list(u_str)
    u_str_array.reverse()

    return string.join(u_str_array, "")

# Return a map for counting pairs e.g. word form and tag pairs.
def get_object_counter():

    return collections.defaultdict(lambda : 0.0)

# Return a map for counting objects, e.g. word forms.
def get_pair_counter():

    return collections.defaultdict(lambda : get_object_counter())

# Return a mapping from pairs in pair_counter to their conditional
# probability given the second member of the pair. The counts of pairs
# are given by pair_counter and the counts of the members by
# object_counter.
def get_conditional_penalty_map(pair_counter, object_counter):

    penalty_map = {}

    for first_member, second_member_dict in pair_counter.iteritems():

        for second_member, pair_count in second_member_dict.iteritems():
            penalty = get_penalty(pair_count, object_counter[second_member])
            penalty_map[(first_member, second_member)] = penalty

    return penalty_map

# return a mapping from objects to their probabilities. The count of
# each object type is given by object_counter and the total number of
# objects is total_count.
def get_penalty_map(object_counter, total_count):

    penalty_map = {}

    for object, count in object_counter.iteritems():

        penalty = get_penalty(count, total_count)
        penalty_map[object] = penalty

    return penalty_map

def print_conditional_penalties(pair_counter, object_counter):

    penalty_map = get_conditional_penalty_map(pair_counter, object_counter) 

    for pair, penalty in penalty_map.iteritems():
        if type(pair[0]) == type(u"") or type(pair[0]) == type(""):
            print string.join([pair[0], pair[1], str(penalty)],"\t")
        else:
            for entry in pair[0]:
                print entry[0] + "\t" + entry[1] + "\t",
            print penalty

def print_penalties(object_counter, total_count):

    penalty_map = get_penalty_map(object_counter, total_count) 

    print penalty_map

    for object, penalty in penalty_map.iteritems():
        print object + "\t" + str(penalty)

