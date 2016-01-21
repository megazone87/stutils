
define copy_inc

$(2): $(1)
	@mkdir -p "$(dir $(2))"
	cp "$(1)" "$(2)"

endef

