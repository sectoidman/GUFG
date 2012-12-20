for x in $(git branch | grep alpha | cut -d '*' -f 2); do git checkout $x; git merge alpha; done
git checkout alpha;
