add_rules("mode.debug", "mode.release")

target("2.1")
    set_kind("binary")
    add_files("src/listings_2.1.cc")
    if is_os("linux") then
        add_links("pthread")
    end

target("2.3")
    set_kind("binary")
    add_files("src/listings_2.3.cc")
    if is_os("linux") then
        add_links("pthread")
    end

target("2.4")
    set_kind("binary")
    add_files("src/listings_2.4.cc")
    set_languages("cxx17")
    if is_os("linux") then
        add_links("pthread")
    end

target("2.5")
    set_kind("binary")
    add_files("src/listings_2.5.cc")
    set_languages("cxx17")
    if is_os("linux") then
        add_links("pthread")
    end

target("2.6")
    set_kind("binary")
    add_files("src/listings_2.6.cc")
    set_languages("cxx17")
    if is_os("linux") then
        add_links("pthread")
    end

target("2.7")
    set_kind("binary")
    add_files("src/listings_2.7.cc")
    set_languages("cxx17")
    if is_os("linux") then
        add_links("pthread")
    end

target("2.8")
    set_kind("binary")
    add_files("src/listings_2.8.cc")
    set_languages("cxx17")
    if is_os("linux") then
        add_links("pthread")
    end

target("3.1")
    set_kind("binary")
    add_files("src/listings_3.1.cc")
    set_languages("cxx17")
    if is_os("linux") then
        add_links("pthread")
    end

target("3.5")
    set_kind("binary")
    add_files("src/listings_3.5.cc")
    set_languages("cxx17")
    if is_os("linux") then
        add_links("pthread")
    end

target("4.1")
    set_kind("binary")
    add_files("src/listings_4.1.cc")
    set_languages("cxx17")
    if is_os("linux") then
        add_links("pthread")
    end

target("4.5")
    set_kind("binary")
    add_files("src/listings_4.5.cc")
    set_languages("cxx17")
    if is_os("linux") then
        add_links("pthread")
    end

target("4.6")
    set_kind("binary")
    add_files("src/listings_4.6.cc")
    set_languages("cxx17")
    if is_os("linux") then
        add_links("pthread")
    end
