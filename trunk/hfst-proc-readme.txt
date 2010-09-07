[This is a short readme file based on an e-mail sent by Brian when the hfst-proc code was moved from the hfst-proc directory to within hfst3. The main purpose is to inform about how to build hfst-proc without the full hfst3 code, see the second paragraph below.]

Hey,
    
    (This is an announcement for everyone using hfst-proc; please forward it to anyone I have overlooked!)
    
    As some of the optimized-lookup transducer code has moved into libhfst proper, and since it does not make sense for hfst-proc to live indefinitely in a separate tree, I have moved hfst-proc into the main hfst development tree (trunk/hfst3/tools/src/hfst-proc) and integrated it with the hfst build system. Work is underway to make hfst buildable with only certain backends enabled, but that is not possible just yet, so here are my current recommendations:
    
    - If you are just using hfst-proc or are using it in conjunction with hfst-lexc and hfst-twolc from hfst2, then continue using the independent hfst-proc, up to revision 624.  Note: This should actually read "up to revision 617". 

[command:
svn up -r617; cd hfst-proc]

    - If you are using more of hfst3 or would like to help test and make sure the migration went smoothly, or if I make bugfixes to hfst-proc that affect you, then you will need to checkout the whole hfst3, which requires you to have OpenFST installed and available on your system. Packages for Debian(-base) systems are available in the Debian experimental repos, downloadable here. I'm using these packages on my Ubuntu 10.04 system. It should hopefully be possible eventually to build a lean hfst with few minimal dependencies.
    
    Let me know if you have any problems getting things working.
    
    --Brian
