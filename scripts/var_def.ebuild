EAPI="1"
DESCRIPTION="SunPinyin is a SLM (Statistical Language Model) based IME"
HOMEPAGE="http://sunpinyin.googlecode.com"
SRC_URI="http://open-gram.googlecode.com/files/dict.utf8.tar.bz2
		http://open-gram.googlecode.com/files/lm_sc.t3g.arpa.tar.bz2"
LICENSE="LGPL-2.1 CDDL"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""
RDEPEND="dev-db/sqlite:3"
DEPEND="${RDEPEND}
		dev-util/pkgconfig"
MY_PATCH=ldflags.patch
PATCH=("1.patch" 2.patch)
ARRAY=(1 2 3 [5]=4)
