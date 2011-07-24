cat<< _EOF_.abc >/dev/null
blah
blah
_EOF_.abc
echo hi

cat > "/dev/null" <<-"EOF"
    #!/bin/sh
    java -Dtijmp.jar="$(java-config -p tijmp)" -agentlib:tijmp "${@}"
EOF

cat > "/dev/null" <<-\EOF
    #!/bin/sh
    java -Dtijmp.jar="$(java-config -p tijmp)" -agentlib:tijmp "${@}"
EOF
