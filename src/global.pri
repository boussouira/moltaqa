
DEVELOPER_BUILD = $$(MOLTAQA_DEV_BUILD)
!isEmpty(DEVELOPER_BUILD) {
    message(Developer build)
    DEFINES += DEV_BUILD
}

exists(../../.git/HEAD) {
    GITVERSION = $$system(git log -n1 --pretty=format:%h)
    !isEmpty(GITVERSION) {
        GITCHANGENUMBER = $$system(git log --pretty=format:%h | wc -l)
        DEFINES += GITVERSION=\"\\\"$$GITVERSION\\\"\"
        DEFINES += GITCHANGENUMBER=\"$$GITCHANGENUMBER\"
    }
}
