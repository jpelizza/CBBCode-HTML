Things are starting to shape up.
As far as I know the simple bbcode is working just fine.
Next up is complex bbcode, which will need a whole new section of the code, probably to be structured like this:
bbcodetohtml(...)

bbcodetohtml(...){
    simple(...);
    complex(...);
    listsimple(...);
    listcomplex(...);
    return (...);
}