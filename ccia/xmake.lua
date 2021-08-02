add_rules("mode.debug", "mode.release")

target("2.1")
    set_kind("binary")
    add_files("src/listings_2.1.cc")

target("2.3")
    set_kind("binary")
    add_files("src/listings_2.3.cc")

target("2.4")
    set_kind("binary")
    add_files("src/listings_2.4.cc")
    set_languages("cxx17")