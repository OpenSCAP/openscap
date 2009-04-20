/* rpminfo_object */
STRUCT(rpminfo_req) {
        PTR(char, name);
};

/* rpminfo_state */
STRUCT(rpminfo_rep) {
        PTR(char, name);
        PTR(char, arch);
        PTR(char, epoch);
        PTR(char, release);
        PTR(char, version);
        PTR(char, evr);
        PTR(char, signature_keyid);
};
