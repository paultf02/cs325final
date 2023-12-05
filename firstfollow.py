# we have 3 categories of term
# nonterminal, terminal, epsilon

grammarversion = "11"

inputgrammarfile = f"./grammars/transformedgrammar{grammarversion}.txt"
outputcsvfile = f"./firstfollow/firstfollowg{grammarversion}.csv"
outputlookaheadfile = f"./grammars/needlookaheadg{grammarversion}.txt"

# sep = "?" # this is a unique symbol that we can use as a separator in cpp for convenience
sep = ", "

inputterminalsfile = "./grammars/terminals2.txt"

with open(inputterminalsfile, "r") as fin:
    terminals = [line.rstrip() for line in fin.readlines()]
with open(inputgrammarfile, "r") as fin:
    lines = fin.readlines()

lhslist = [] # list of nonterminals in order of production
rhslist = []
index = {}
for i in range(len(lines)):
    a = lines[i].rstrip().split(" -> ")
    production, fullrhs = a
    rhs = [term.split(" ") for term in fullrhs.split(" | ")]
    # rhslist = [list of rhs]
    # rhs = [list of or sections]
    # or_sections = [list of words]
    lhslist.append(production)
    rhslist.append(rhs)
    index[production] = i

is_terminal = {}
for nonterminal in lhslist:
    is_terminal[nonterminal] = False
for terminal in terminals:
    is_terminal[terminal] = True
#print(nonterminals)

# for i in range(len(rhslist)):
#     print(f"{lhslist[i]} is the production, rhs is a {type(rhslist[i])} it has {len(rhslist[i])} options")

#print(rhslist[29][1])

##################NULLABLE CODE

nullable = {term: None for term in lhslist}
nullable["epsilon"] = True
for t in terminals:
    nullable[t] = False
#print(nullable)

def findnullable(term):
    if nullable[term] != None:
        return nullable[term]
    else:
        #print(term)
        i = index[term]
        rhs = rhslist[i]
        term_nullable = False
        for or_statement in rhs:
            or_statement_nullable = True
            for atom in or_statement:
                or_statement_nullable = or_statement_nullable & findnullable(atom)
                if or_statement_nullable == False:
                    break
            term_nullable = term_nullable | or_statement_nullable
        nullable[term] = term_nullable
        return nullable[term]

print("started computing which elements are nullable")
for term in nullable.keys():
    if nullable[term] == None:
        findnullable(term)
print("finished computing which elements are nullable")
    

# print("The nullable elements are:")
# [print(production) for production in nullable.keys() if nullable[production]]
# print("The non nullable elements are:")
# [print(production) for production in nullable.keys() if nullable[production] == False]
# print("The elements whose nullability has not been calculated are:")
# [print(production) for production in nullable.keys() if nullable[production] == None]

##################FIRST SET CODE

first = {term: None for term in lhslist}
first["epsilon"] = set()
for t in terminals:
    first[t] = set([t])

def sentencenull(or_statement):
    for atom in or_statement:
        if nullable[atom] == False:
            return False
    return True
    
def findfirst(term):
    if first[term] != None:
        #epsilon and terminals should exit here
        return first[term]
    else:
        #print(term)
        #only non terminals go further than here
        thisfirstset = set()
        i = index[term]
        rhs = rhslist[i]
        
        for or_statement in rhs:
            if or_statement == ["epsilon"]:
                #we need this because epsilon has empty first set
                thisfirstset.add("epsilon")
            
            #in this loop we find orfirstset
            orfirstset = set()
            allnull = True
            for atom in or_statement:
                if nullable[atom] == False:
                    orfirstset = orfirstset.union(findfirst(atom))
                    allnull = False
                    break
                elif nullable[atom] == True:
                    orfirstset = orfirstset.union(findfirst(atom) - set(["epsilon"]))
            if allnull == True:
                orfirstset.add("epsilon")
            
            thisfirstset = thisfirstset.union(orfirstset)
        first[term] = thisfirstset
        return first[term]


print("starting computing first sets")
for term in first.keys():
    if first[term] == None:
        findfirst(term)
    #print(f"{term} has first set:\n{first[term]}")
print("finished computing first sets")

##################FOLLOW SET CODE

# we say follow set of A is a union of other follow sets and first sets
# first sets already computed.
# if follow(A) is a subset of follow(B) but follow(A)
# has not been established what do we do
# the only thing that effects follow(A) is the presence of
# A on the RHS
# after the last occurence on the RHS we can then compute follow(A)
# go through code from top to bottom
# for a fixed A
# follow set dependencies is a list
# first set dependencies is a list
# now order the terms by their last appearance

print("starting to build follow set dependencies")

last_appearance = {nonterminal: -1 for nonterminal in lhslist}
for linenum in range(len(lhslist)):
    rhs = rhslist[linenum]
    for orsequence in rhs:
        for atom in orsequence:
            if atom != "epsilon" and is_terminal[atom] == False:
                last_appearance[atom] = linenum
#below tells us order to compute follow sets
lhssorted = sorted(last_appearance.items(), key=(lambda x: x[1]))
lhssorted = list(map(lambda x: x[0], lhssorted))
#[print(f"{key}: {value}") for (key, value) in last_appearance.items()]
dependencies = {lhs: {"follow":set(), "first":set()} for lhs in lhslist} # follow_dependencies, first_dependencies

for linenum in range(len(rhslist)):
    lhs = lhslist[linenum]
    rhs = rhslist[linenum]
    for orsequence in rhs:
        l = len(orsequence)
        for i in range(l):
            thisatom = orsequence[i]
            if (thisatom == "epsilon") or (is_terminal[thisatom] == True):
                continue
            if (i < l-1):
                nextatom = orsequence[i+1]                
                for j in range(i+1, l):
                    dependencies[thisatom]["first"].add(nextatom)
                    if nullable[orsequence[j]] == False:
                        break
                if sentencenull(orsequence[i+1:]) == True:
                    dependencies[thisatom]["follow"].add(lhs)
            elif i == l-1:
                dependencies[thisatom]["follow"].add(lhs)
            dependencies[thisatom]["follow"] = dependencies[thisatom]["follow"] - set([thisatom])

print("finished building follow set dependencies")
#[print(f"{key}: {value}") for (key, value) in dependencies.items()]
#[print(f"{lhs}: {dependencies[lhs]}") for lhs in lhssorted]

# We have to deal with two problems
# If followset(A) depends on followset(B) but B is after A in lhssorted
# this means last appearance of B is after last appearance of A

# this dependency graph has nodes that are sets
# there is an arrow from the node of set X to the node of set Y if
# set X contains set Y
# there are two types of set that can be a node.
# a node could be a follow set. this can have some or no outgoing arrows
# a node could be a first set. this definitely has 0 outgoing arrows since we have already computed it
# if we have a cycle then we need to contract all edges in the cycle
# we keep contracting edges until the graph has no cycles
# note that the number of connected components never change throughout this process of contracting

# how do we do this in code

# let G = dependencies
# let A1, ..., Ak be a cycle k>=2
# Let Gprime = G
# union all the follow and first dependencies of A1,...,Ak and remove A1,...,Ak followdeps to get followdeps, firstdeps
# delete A1,...,Ak from Gprime
# add the tuple (A1,...,Ak) to Gprimes keys with followdeps and first deps
# repeat to get a final graph G with no cycles

print("contracting edges till dependency digraph has no cycles")

def get_cycle(stack, vertex, detected_cycles):
    # vertex is in stack
    # we compute the cycle from the top of the stack to vertex 
    # after the algorithm is done the stack is popped till vertex is the last one to be popped
    cycle = []
    cycle.append(stack[-1])
    stack.pop()
    while cycle[-1] != vertex:
        cycle.append(stack[-1])
        stack.pop()
    if cycle not in detected_cycles:
        detected_cycles.append(cycle)
    return stack, detected_cycles

def processDFSTree(graph, stack, visited, detected_cycles):
    #print(f"stack at the beginning of call: {stack}")
    if len(stack) == 0:
        #print("stack is empty")
        return stack, visited, detected_cycles
    #print(f"the top of the stack is {stack[-1]}")
    for vertex in graph[stack[-1]]["follow"]:
        # looking at the children of the top of the stack
        if visited[vertex] == "in_stack":
            _, detected_cycles = get_cycle(stack.copy(), vertex, detected_cycles)
            # now stack is pointing to below vertex (possibly empty)
            # print(f"we have detected the following cycles: {detected_cycles}")
            #print(f"stack: {stack}")
        elif visited[vertex] == "not_visited":
            stack.append(vertex)
            # newstack = stack.copy() + [vertex]
            visited[vertex] = "in_stack"
            stack, visited, detected_cycles = processDFSTree(graph, stack.copy(), visited, detected_cycles)
    
    x = [name for (name, val) in visited.items() if val != "not_visited"]
    #print(f"the stack here is: {stack}\nvisited is: {x}")
    #print(f"the stack here is: {stack}")
    visited[stack[-1]] = "visited"
    stack.pop()
    #print(f"stack at the end of the call: {stack}")
    return stack, visited, detected_cycles



def find_cycle(graph):
    visited = {v: "not_visited" for v in graph.keys()}
    for vertex in graph.keys():
        if visited[vertex] == "not_visited":
            stack = []
            stack.append(vertex)
            visited[vertex] = "in_stack"
            detected_cycles = []
            #stack points to vertex and this should always be at the base of the stack
            stack, visited, detected_cycles = processDFSTree(graph, stack, visited, detected_cycles)
            if len(detected_cycles) > 0:
                #actually this is a hack
                cycle = max(detected_cycles, key=len)
                return cycle
    return None

G = dependencies.copy()
availablename = 0
nametoterms = {}
# print(f"the graph is:")
# [print(f"{key}: {value}") for (key, value) in G.items()]
while True:
    print("finding cycle")
    cycle = find_cycle(G) # note that cycle can only be where all nodes in cycle are follow sets
    print(f"availablename: {availablename}, cycle: {cycle}")
    if cycle == None:
        break
    
    Gprime = G.copy()
    newfollowdeps = set()
    newfirstdeps = set()
    for node in cycle:
        newfollowdeps.update(G[node]["follow"])
        newfirstdeps.update(G[node]["first"])
        Gprime.pop(node)
    newfollowdeps = newfollowdeps - set(cycle)
    Gprime[availablename] = {"follow":set(), "first":set()}
    Gprime[availablename]["follow"] = newfollowdeps.copy()
    Gprime[availablename]["first"] = newfirstdeps.copy()
    nametoterms[availablename] = cycle.copy()

    #now we need to replace any reference in follow to one of the elements of cycle with a reference to available name
    for vertex in Gprime.keys():
        for x in Gprime[vertex]["follow"]:
            if x in cycle:
                Gprime[vertex]["follow"].remove(x)
                Gprime[vertex]["follow"].add(availablename)
    print("graph contracted")
    availablename += 1
    G = Gprime.copy()
    # print(f"the graph is:")
    # [print(f"{key}: {value}") for (key, value) in G.items()]
    # print(nametoterms)


# print("our final graphs is:")
# [print(f"{key}: {value}") for (key, value) in G.items()]
# print("the key for new nodes is:")
# [print(f"{key}: {value}") for (key, value) in nametoterms.items()]

# now we need to topological sort the vertices of G


def topsortutil(graph, vertex, visited, stack):
    #print(vertex)
    #print(stack)
    visited[vertex] = True
    for newvertex in graph[vertex]["follow"]:
        #print(f"new vertex is: {newvertex}")
        if visited[newvertex] == False:
            visited, stack = topsortutil(graph, newvertex, visited, stack.copy())
    stack.insert(0, vertex)
    return visited, stack

def topsort(graph):
    visited = {vertex: False for vertex in graph.keys()}
    stack = []
    for vertex in graph.keys():
        if visited[vertex] == False:
            visited, stack = topsortutil(graph, vertex, visited, stack.copy())
    return stack

tsortedvertices = list(reversed(topsort(G)))
# print("the topological sorted vertices of the graph are:")
# [print(v) for v in tsortedvertices]
#print(tsortedvertices)


follow = {lhs: None for lhs in tsortedvertices}
#now compute follow sets
print("starting computing follow sets")
for nonterminal in tsortedvertices:
    #print("hi")
    #print(f"{nonterminal} follow:")
    thisfollowset = set()
    followdep = G[nonterminal]["follow"]
    firstdep = G[nonterminal]["first"]
    for a in firstdep:
        thisfollowset.update(first[a] - set(["epsilon"]))
        # if "epsilon" in (first[a] - set(["epsilon"])):
        #     print("we have an error here")
    for a in followdep:
        if a != nonterminal:
            thisfollowset.update(follow[a])
            # if "epsilon" in (follow[a]):
            #     print(f"we have an error here, a={a}")

    # if "epsilon" in thisfollowset:
    #     print(f"epsilon in followset for {nonterminal}")
    #     print(f"followset is:\n{thisfollowset}")
    #     print(f"dependencies are:\n{dependencies[nonterminal]}")
    #     raise ValueError
    follow[nonterminal] = thisfollowset
    #print(follow[nonterminal])
print("finished computing follow sets")

print("modifying so that a multiple terminal node is split")
keystopop = [key for key in follow.keys() if (key in nametoterms.keys())]
#replacekeywith = {key: [] for key in keystopop.keys()}

def get_replacekeywith(keystopop, nametoterms):
    replacekeywith = {key: None for key in keystopop}
    for key in keystopop:
        ourlist = nametoterms[key].copy()
        while True:
            to_replace = [name for name in ourlist if name in nametoterms.keys()]
            if len(to_replace) ==0:
                break
            #remove only removes the first occurrence but it should only have one
            for name in to_replace:
                ourlist.remove(name)
                ourlist.extend(nametoterms[name])
        replacekeywith[key] = ourlist
    return replacekeywith

replacekeywith = get_replacekeywith(keystopop, nametoterms)

for key, replacementlist in replacekeywith.items():
    thisfollowset = follow[key]
    follow.pop(key)
    for newkey in replacementlist:
        follow[newkey] = thisfollowset
    

# print("the follow sets are:")
# [print(f"{key}: {value}") for (key, value) in follow.items()]

print(f"number of terms we computed followsets for: {len(follow.keys())}, number of terms in lhslist: {len(lhslist)}")
assert len(follow.keys()) == len(lhslist)


for t in terminals:
    follow[t] = set(["N/A"])
follow["epsilon"] = set(["N/A"])
#save this data to file
#csv in the form
#term,nullable,firstset,followset
def to_string(thisset):
    string = '"'
    for term in thisset:
        if term[0] == '"':
            string += term[1:-1]
        else:
            string += term
        string += ","
    if len(string) > 1:
        string = string[0:-1] #remove last comma if thisset not empty
    #print(string)
    #string.replace('"', "'")
    #print(string)
    string += '"'
    return string


ostring = f"term{sep}nullable{sep}firstset{sep}followset\n"
for term in first.keys():
    #print(first[term])
    thisrow = f"{term}{sep}{nullable[term]}{sep}{to_string(first[term])}{sep}{to_string(follow[term])}\n"
    ostring += thisrow
    #print(thisrow)
with open(outputcsvfile, 'w+') as fout:
    fout.write(ostring)
    print("first and follow sets saved into csv file")


print("starting building parser table")

def sentence_first(sentence):
    firstset = set()
    for word in sentence:
        if nullable[word]:
            firstset.update(first[word] - set(["epsilon"]))
        else:
            firstset.update(first[word])
            return firstset
    if sentencenull(sentence):
        firstset.add("epsilon")
    return firstset
    

parser_table = {(nonterminal, terminal): [] for nonterminal in lhslist for terminal in terminals}
for lhs in lhslist:
    for terminal in terminals:
        for i in range(len(rhslist)):
            rhs = rhslist[i]
            # trying this
            if lhslist[i] != lhs:
                continue
            for j in range(len(rhs)):
                or_sequence = rhs[j]
                if terminal in sentence_first(or_sequence):
                    if (i, j) not in parser_table[(lhs, terminal)]:
                        parser_table[(lhs, terminal)].append(("first",i,j))
                if sentencenull(or_sequence) and (terminal in follow[lhs]):
                    if (i, j) not in parser_table[(lhs, terminal)]:
                        #print("used nullable condition")
                        parser_table[(lhs, terminal)].append(("nullable",i,j))

print("finished building parser table")
t0 = [f"{key} : {value}" for key, value in parser_table.items() if len(value) == 0]
t1 = [f"{key} : {value}" for key, value in parser_table.items() if len(value) == 1]
tm = [f"{key} : {value}" for key, value in parser_table.items() if len(value) > 1]
need_lookahead_str = ""
need_lookahead_str += f"{len(lhslist)} terms in lhs of grammar * {len(terminals)} terminals = {len(lhslist)*len(terminals)}, there are {len(parser_table.keys())} cells in the table\n"
need_lookahead_str += f"{len(t0)} cells with 0 entries\n{len(t1)} cells with 1 entry\n{len(tm)} cells with more than 1 entry\n"
need_lookahead_str += "cells with more than 1 entry:\n"
for item in tm:
    need_lookahead_str += (item + '\n')

with open(outputlookaheadfile, 'w+') as fout2:
    print(need_lookahead_str)
    fout2.write(need_lookahead_str)
    print("Done saving which productions need lookahead in "+outputlookaheadfile)
