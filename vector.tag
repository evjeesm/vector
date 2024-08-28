<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.11.0" doxygen_gitid="9b424b03c9833626cd435af22a444888fbbb192d">
  <compound kind="file">
    <name>vector.c</name>
    <path>src/</path>
    <filename>vector_8c.html</filename>
    <includes id="vector_8h" name="vector.h" local="yes" import="no" module="no" objc="no">vector.h</includes>
    <class kind="struct">vector_t</class>
    <member kind="function" static="yes">
      <type>static size_t</type>
      <name>calculate_alloc_size</name>
      <anchorfile>vector_8c.html</anchorfile>
      <anchor>a340abc280ca3260619357acb41f2aa98</anchor>
      <arglist>(const size_t element_size, const size_t capacity, const size_t allocator_size, const size_t ext_header_size)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void *</type>
      <name>get_allocator</name>
      <anchorfile>vector_8c.html</anchorfile>
      <anchor>a4511ed374861bb6242bc2b6791012ba4</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void *</type>
      <name>binary_find</name>
      <anchorfile>vector_8c.html</anchorfile>
      <anchor>afec86be827dcdde96c62016b77630a7c</anchor>
      <arglist>(const vector_t *const vector, const void *const value, const size_t start, const size_t end, const compare_t cmp, void *const param)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static ssize_t</type>
      <name>binary_find_index</name>
      <anchorfile>vector_8c.html</anchorfile>
      <anchor>a162bfa9cf07c25a5eb6c883820a68e99</anchor>
      <arglist>(const vector_t *const vector, const void *const value, const size_t start, const size_t end, const compare_t cmp, void *param)</arglist>
    </member>
    <member kind="function">
      <type>vector_t *</type>
      <name>vector_create_</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>gae9c418efba1bbeb20a8becffe6a09b75</anchor>
      <arglist>(const vector_opts_t *const opts)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_destroy</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>gae0c0da17ad9469e47ef376a82b6493f3</anchor>
      <arglist>(vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>vector_t *</type>
      <name>vector_clone</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>ga7243756372efd57714853efaf7521358</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>vector_status_t</type>
      <name>vector_resize</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>ga6e5077303af4dfe720af5a0608553922</anchor>
      <arglist>(vector_t **const vector, const size_t capacity, const vector_status_t error)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_get_ext_header</name>
      <anchorfile>group__Extension.html</anchorfile>
      <anchor>gafc2e34dbacffe6480af8681fbf46e808</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_ext_header_size</name>
      <anchorfile>group__Extension.html</anchorfile>
      <anchor>gacb4ec311fa5851f503419118b4984f54</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_data_offset</name>
      <anchorfile>group__Extension.html</anchorfile>
      <anchor>ga435ecb8dca435624d8d277feb6b2cc80</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_copy</name>
      <anchorfile>group__Vector__API.html</anchorfile>
      <anchor>gabf21611afbbdda61da4ffa361fc783dc</anchor>
      <arglist>(const vector_t *const vector, char *const dest, const size_t offset, const size_t length)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_move</name>
      <anchorfile>group__Vector__API.html</anchorfile>
      <anchor>gaa94c547c9488b4f9dce5eb01067c2ae1</anchor>
      <arglist>(const vector_t *const vector, char *dest, const size_t offset, const size_t length)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_part_copy</name>
      <anchorfile>group__Vector__API.html</anchorfile>
      <anchor>ga39836c927f0e0fc79dbf39728972d08e</anchor>
      <arglist>(const vector_t *const vector, char *dest, const size_t offset, const size_t length, const size_t part_offset, const size_t part_length)</arglist>
    </member>
    <member kind="function">
      <type>alloc_opts_t</type>
      <name>vector_alloc_opts</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>gafe6a5c3c08ddafc9be60051f447be0e1</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_element_size</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>gaae93d7f0c056c1d11d3232a7a7aa5e09</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_capacity</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>gab24bd801f91d4ae1ac79b54430b0409f</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_capacity_bytes</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>ga3bfc1317eaeaee9b29a452c4fa41e4b3</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_linear_find</name>
      <anchorfile>group__Searches.html</anchorfile>
      <anchor>ga184f04299040b2bfd8c03ef22abc7b8b</anchor>
      <arglist>(const vector_t *const vector, const size_t limit, const predicate_t predicate, void *param)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_binary_find</name>
      <anchorfile>group__Searches.html</anchorfile>
      <anchor>ga5081815905cf71570856aef717659463</anchor>
      <arglist>(const vector_t *const vector, const void *const value, const size_t limit, const compare_t cmp, void *param)</arglist>
    </member>
    <member kind="function">
      <type>ssize_t</type>
      <name>vector_binary_find_index</name>
      <anchorfile>group__Searches.html</anchorfile>
      <anchor>gaae3cfb2691a50da87169b5e057169782</anchor>
      <arglist>(const vector_t *const vector, const void *const value, const size_t limit, const compare_t cmp, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>vector_data</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gac4d24b2cc8c8c44d36107b9a0fffea35</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_get</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga9437d9506a3b2799f595c88b82f160b3</anchor>
      <arglist>(const vector_t *const vector, const size_t index)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_set</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gab3a5e466261d31ee9edd77a5eb280030</anchor>
      <arglist>(vector_t *const vector, const size_t index, const void *const value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_set_zero</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gaf3eaea4a3a47741f46eae9ae6e984ef2</anchor>
      <arglist>(vector_t *const vector, const size_t index)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_spread</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga6312527827ba72441feeef4fc4047f94</anchor>
      <arglist>(vector_t *const vector, const size_t index, const size_t amount)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_shift</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga1f4cbce56499e81a048ac6454cd5bb69</anchor>
      <arglist>(vector_t *const vector, const size_t offset, const size_t length, const ssize_t shift)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_swap</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gaf395f309bc15469998a8c14fc925b5b6</anchor>
      <arglist>(vector_t *const vector, const size_t index_a, const size_t index_b)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>vector_foreach</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga988880baa4fe32fb6d4299daf1a29a1f</anchor>
      <arglist>(const vector_t *const vector, const size_t limit, const foreach_t func, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>vector_aggregate</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga1a193e06f6700de896628631b8569740</anchor>
      <arglist>(const vector_t *const vector, const size_t limit, const aggregate_t func, void *const acc, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>vector_transform</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga176c007bd441e4469a28ba47ca952dfb</anchor>
      <arglist>(vector_t *const vector, const size_t limit, const transform_t func, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>calc_aligned_size</name>
      <anchorfile>group__Utilities.html</anchorfile>
      <anchor>gaacab7d4483d92f4ca41fbc548f5eab09</anchor>
      <arglist>(const size_t size, const size_t alignment)</arglist>
    </member>
    <member kind="function">
      <type>ssize_t</type>
      <name>cmp_lex_asc</name>
      <anchorfile>group__Utilities.html</anchorfile>
      <anchor>ga4b46f84c281c8dbe3e7e0e706d25053e</anchor>
      <arglist>(const void *value, const void *element, void *param)</arglist>
    </member>
    <member kind="function">
      <type>ssize_t</type>
      <name>cmp_lex_dsc</name>
      <anchorfile>group__Utilities.html</anchorfile>
      <anchor>gab1d75d913a83778eb75c079544c4e4ae</anchor>
      <arglist>(const void *value, const void *element, void *param)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>vector.h</name>
    <path>src/</path>
    <filename>vector_8h.html</filename>
    <class kind="struct">alloc_opts_t</class>
    <class kind="struct">vector_opts_t</class>
    <member kind="define">
      <type>#define</type>
      <name>TMP_REF</name>
      <anchorfile>vector_8h.html</anchorfile>
      <anchor>ad92ce696967261588cab3848dfe73c02</anchor>
      <arglist>(type, value)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>VECTOR_DEFAULT_ARGS</name>
      <anchorfile>vector_8h.html</anchorfile>
      <anchor>a5e34f2d91394e1cfcde918615affbc86</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>alloc_opts</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>gaa28827c8d5d9cf48741aaa05e8fd608a</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>vector_create</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>ga160b0d4b8d0836eda9a50bfa71514de8</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="typedef">
      <type>bool(*</type>
      <name>predicate_t</name>
      <anchorfile>group__Callbacks.html</anchorfile>
      <anchor>ga61d45c5be2935b552d99c0c74b7973ef</anchor>
      <arglist>)(const void *const element, void *const param)</arglist>
    </member>
    <member kind="typedef">
      <type>ssize_t(*</type>
      <name>compare_t</name>
      <anchorfile>group__Callbacks.html</anchorfile>
      <anchor>gae47f476ece20d219ac1d39c71110e553</anchor>
      <arglist>)(const void *const value, const void *const element, void *const param)</arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>foreach_t</name>
      <anchorfile>group__Callbacks.html</anchorfile>
      <anchor>ga2a12fb990fb59f84ca1920d78d867aa6</anchor>
      <arglist>)(const void *const element, void *const param)</arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>aggregate_t</name>
      <anchorfile>group__Callbacks.html</anchorfile>
      <anchor>ga89204e5e91ceae8be993751982edb299</anchor>
      <arglist>)(const void *const element, void *const acc, void *const param)</arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>transform_t</name>
      <anchorfile>group__Callbacks.html</anchorfile>
      <anchor>ga3b3896688782a9a532d27c5d841c0e79</anchor>
      <arglist>)(void *const element, void *const param)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>vector_status_t</name>
      <anchorfile>vector_8h.html</anchorfile>
      <anchor>ab79a65b74cda0380395a1ece246f54c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>VECTOR_SUCCESS</name>
      <anchorfile>vector_8h.html</anchorfile>
      <anchor>ab79a65b74cda0380395a1ece246f54c6a4efd6ebea3161f7266fb6debb2f1768a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>VECTOR_ALLOC_ERROR</name>
      <anchorfile>vector_8h.html</anchorfile>
      <anchor>ab79a65b74cda0380395a1ece246f54c6a54fc9ef540c1d725565596440632fb2c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>VECTOR_STATUS_LAST</name>
      <anchorfile>vector_8h.html</anchorfile>
      <anchor>ab79a65b74cda0380395a1ece246f54c6a58c175c9d5dcfe5018f8981b5ee72bd9</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>vector_t *</type>
      <name>vector_create_</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>gae9c418efba1bbeb20a8becffe6a09b75</anchor>
      <arglist>(const vector_opts_t *const opts)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_destroy</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>gae0c0da17ad9469e47ef376a82b6493f3</anchor>
      <arglist>(vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>vector_t *</type>
      <name>vector_clone</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>ga7243756372efd57714853efaf7521358</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>vector_status_t</type>
      <name>vector_resize</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>ga6e5077303af4dfe720af5a0608553922</anchor>
      <arglist>(vector_t **const vector, const size_t capacity, const vector_status_t error)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_get_ext_header</name>
      <anchorfile>group__Extension.html</anchorfile>
      <anchor>gafc2e34dbacffe6480af8681fbf46e808</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_ext_header_size</name>
      <anchorfile>group__Extension.html</anchorfile>
      <anchor>gacb4ec311fa5851f503419118b4984f54</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_data_offset</name>
      <anchorfile>group__Extension.html</anchorfile>
      <anchor>ga435ecb8dca435624d8d277feb6b2cc80</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_copy</name>
      <anchorfile>group__Vector__API.html</anchorfile>
      <anchor>gabf21611afbbdda61da4ffa361fc783dc</anchor>
      <arglist>(const vector_t *const vector, char *dest, const size_t offset, const size_t length)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_move</name>
      <anchorfile>group__Vector__API.html</anchorfile>
      <anchor>gaa94c547c9488b4f9dce5eb01067c2ae1</anchor>
      <arglist>(const vector_t *const vector, char *dest, const size_t offset, const size_t length)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_part_copy</name>
      <anchorfile>group__Vector__API.html</anchorfile>
      <anchor>ga39836c927f0e0fc79dbf39728972d08e</anchor>
      <arglist>(const vector_t *const vector, char *dest, const size_t offset, const size_t length, const size_t part_offset, const size_t part_length)</arglist>
    </member>
    <member kind="function">
      <type>alloc_opts_t</type>
      <name>vector_alloc_opts</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>gafe6a5c3c08ddafc9be60051f447be0e1</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_element_size</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>gaae93d7f0c056c1d11d3232a7a7aa5e09</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_capacity</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>gab24bd801f91d4ae1ac79b54430b0409f</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_capacity_bytes</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>ga3bfc1317eaeaee9b29a452c4fa41e4b3</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_linear_find</name>
      <anchorfile>group__Searches.html</anchorfile>
      <anchor>ga184f04299040b2bfd8c03ef22abc7b8b</anchor>
      <arglist>(const vector_t *const vector, const size_t limit, const predicate_t predicate, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_binary_find</name>
      <anchorfile>group__Searches.html</anchorfile>
      <anchor>ga5081815905cf71570856aef717659463</anchor>
      <arglist>(const vector_t *const vector, const void *const value, const size_t limit, const compare_t cmp, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>ssize_t</type>
      <name>vector_binary_find_index</name>
      <anchorfile>group__Searches.html</anchorfile>
      <anchor>gaae3cfb2691a50da87169b5e057169782</anchor>
      <arglist>(const vector_t *const vector, const void *const value, const size_t limit, const compare_t cmp, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>vector_data</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gac4d24b2cc8c8c44d36107b9a0fffea35</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_get</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga9437d9506a3b2799f595c88b82f160b3</anchor>
      <arglist>(const vector_t *const vector, const size_t index)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_set</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gab3a5e466261d31ee9edd77a5eb280030</anchor>
      <arglist>(vector_t *const vector, const size_t index, const void *const value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_set_zero</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gaf3eaea4a3a47741f46eae9ae6e984ef2</anchor>
      <arglist>(vector_t *const vector, const size_t index)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_spread</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga6312527827ba72441feeef4fc4047f94</anchor>
      <arglist>(vector_t *const vector, const size_t index, const size_t amount)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_shift</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga1f4cbce56499e81a048ac6454cd5bb69</anchor>
      <arglist>(vector_t *const vector, const size_t offset, const size_t length, const ssize_t shift)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_swap</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gaf395f309bc15469998a8c14fc925b5b6</anchor>
      <arglist>(vector_t *const vector, const size_t index_a, const size_t index_b)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>vector_foreach</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga988880baa4fe32fb6d4299daf1a29a1f</anchor>
      <arglist>(const vector_t *const vector, const size_t limit, const foreach_t func, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>vector_aggregate</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga1a193e06f6700de896628631b8569740</anchor>
      <arglist>(const vector_t *const vector, const size_t limit, const aggregate_t func, void *const acc, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>vector_transform</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga176c007bd441e4469a28ba47ca952dfb</anchor>
      <arglist>(vector_t *const vector, const size_t limit, const transform_t func, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_alloc</name>
      <anchorfile>group__Allocation.html</anchorfile>
      <anchor>gac06d4923e85171dfa1e3fae4f141b658</anchor>
      <arglist>(const size_t alloc_size, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_realloc</name>
      <anchorfile>group__Allocation.html</anchorfile>
      <anchor>ga6bc2c0ee8e64283e06259c3edc6c6022</anchor>
      <arglist>(void *ptr, const size_t alloc_size, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_free</name>
      <anchorfile>group__Allocation.html</anchorfile>
      <anchor>gacd97ea77de38db5fb50d9e8ba216a734</anchor>
      <arglist>(void *ptr, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>calc_aligned_size</name>
      <anchorfile>group__Utilities.html</anchorfile>
      <anchor>gaacab7d4483d92f4ca41fbc548f5eab09</anchor>
      <arglist>(const size_t size, const size_t alignment)</arglist>
    </member>
    <member kind="function">
      <type>ssize_t</type>
      <name>cmp_lex_asc</name>
      <anchorfile>group__Utilities.html</anchorfile>
      <anchor>ga4b46f84c281c8dbe3e7e0e706d25053e</anchor>
      <arglist>(const void *const value, const void *const element, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>ssize_t</type>
      <name>cmp_lex_dsc</name>
      <anchorfile>group__Utilities.html</anchorfile>
      <anchor>gab1d75d913a83778eb75c079544c4e4ae</anchor>
      <arglist>(const void *const value, const void *const element, void *const param)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>alloc_opts_t</name>
    <filename>structalloc__opts__t.html</filename>
    <member kind="variable">
      <type>size_t</type>
      <name>size</name>
      <anchorfile>structalloc__opts__t.html</anchorfile>
      <anchor>abcbbae68ec835dd4091a32487bc9daa2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>data</name>
      <anchorfile>structalloc__opts__t.html</anchorfile>
      <anchor>ada6f016cb67723977851f966ca0f947f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>vector_opts_t</name>
    <filename>structvector__opts__t.html</filename>
    <member kind="variable">
      <type>alloc_opts_t</type>
      <name>alloc_opts</name>
      <anchorfile>structvector__opts__t.html</anchorfile>
      <anchor>a390f740c675ad2161cb604d72e5957ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>ext_header_size</name>
      <anchorfile>structvector__opts__t.html</anchorfile>
      <anchor>ae5a8b3368fff2365e65d2816f7a57ecd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>element_size</name>
      <anchorfile>structvector__opts__t.html</anchorfile>
      <anchor>a18f1f3e195d1c4a691e232b1fcaec0c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>initial_cap</name>
      <anchorfile>structvector__opts__t.html</anchorfile>
      <anchor>a3bcfe6caf89c0029a925d0be842abc23</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>vector_t</name>
    <filename>structvector__t.html</filename>
    <member kind="variable">
      <type>size_t</type>
      <name>element_size</name>
      <anchorfile>structvector__t.html</anchorfile>
      <anchor>ada42e4f54768423800961ae3e815406d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>capacity</name>
      <anchorfile>structvector__t.html</anchorfile>
      <anchor>a81e18fd00e94087a8f04b657ffc34c04</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>ext_header_size</name>
      <anchorfile>structvector__t.html</anchorfile>
      <anchor>ad039a1d8cf3619808dcf2bfd04fc4ded</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>allocator_size</name>
      <anchorfile>structvector__t.html</anchorfile>
      <anchor>a158574d167ee9598bf51b1eee2a3c575</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>memory</name>
      <anchorfile>structvector__t.html</anchorfile>
      <anchor>a2c7a23fb3c2197cc5557b8543ffdb2f7</anchor>
      <arglist>[]</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>Vector_API</name>
    <title>Vector API</title>
    <filename>group__Vector__API.html</filename>
    <subgroup>Callbacks</subgroup>
    <subgroup>Lifetime</subgroup>
    <subgroup>Extension</subgroup>
    <subgroup>Properties</subgroup>
    <subgroup>Searches</subgroup>
    <subgroup>Elements</subgroup>
    <subgroup>Allocation</subgroup>
    <subgroup>Utilities</subgroup>
    <member kind="function">
      <type>void</type>
      <name>vector_copy</name>
      <anchorfile>group__Vector__API.html</anchorfile>
      <anchor>gabf21611afbbdda61da4ffa361fc783dc</anchor>
      <arglist>(const vector_t *const vector, char *dest, const size_t offset, const size_t length)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_move</name>
      <anchorfile>group__Vector__API.html</anchorfile>
      <anchor>gaa94c547c9488b4f9dce5eb01067c2ae1</anchor>
      <arglist>(const vector_t *const vector, char *dest, const size_t offset, const size_t length)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_part_copy</name>
      <anchorfile>group__Vector__API.html</anchorfile>
      <anchor>ga39836c927f0e0fc79dbf39728972d08e</anchor>
      <arglist>(const vector_t *const vector, char *dest, const size_t offset, const size_t length, const size_t part_offset, const size_t part_length)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>Callbacks</name>
    <title>Callbacks</title>
    <filename>group__Callbacks.html</filename>
    <member kind="typedef">
      <type>bool(*</type>
      <name>predicate_t</name>
      <anchorfile>group__Callbacks.html</anchorfile>
      <anchor>ga61d45c5be2935b552d99c0c74b7973ef</anchor>
      <arglist>)(const void *const element, void *const param)</arglist>
    </member>
    <member kind="typedef">
      <type>ssize_t(*</type>
      <name>compare_t</name>
      <anchorfile>group__Callbacks.html</anchorfile>
      <anchor>gae47f476ece20d219ac1d39c71110e553</anchor>
      <arglist>)(const void *const value, const void *const element, void *const param)</arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>foreach_t</name>
      <anchorfile>group__Callbacks.html</anchorfile>
      <anchor>ga2a12fb990fb59f84ca1920d78d867aa6</anchor>
      <arglist>)(const void *const element, void *const param)</arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>aggregate_t</name>
      <anchorfile>group__Callbacks.html</anchorfile>
      <anchor>ga89204e5e91ceae8be993751982edb299</anchor>
      <arglist>)(const void *const element, void *const acc, void *const param)</arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>transform_t</name>
      <anchorfile>group__Callbacks.html</anchorfile>
      <anchor>ga3b3896688782a9a532d27c5d841c0e79</anchor>
      <arglist>)(void *const element, void *const param)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>Lifetime</name>
    <title>Lifetime</title>
    <filename>group__Lifetime.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>alloc_opts</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>gaa28827c8d5d9cf48741aaa05e8fd608a</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>vector_create</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>ga160b0d4b8d0836eda9a50bfa71514de8</anchor>
      <arglist>(...)</arglist>
    </member>
    <member kind="function">
      <type>vector_t *</type>
      <name>vector_create_</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>gae9c418efba1bbeb20a8becffe6a09b75</anchor>
      <arglist>(const vector_opts_t *const opts)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_destroy</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>gae0c0da17ad9469e47ef376a82b6493f3</anchor>
      <arglist>(vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>vector_t *</type>
      <name>vector_clone</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>ga7243756372efd57714853efaf7521358</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>vector_status_t</type>
      <name>vector_resize</name>
      <anchorfile>group__Lifetime.html</anchorfile>
      <anchor>ga6e5077303af4dfe720af5a0608553922</anchor>
      <arglist>(vector_t **const vector, const size_t capacity, const vector_status_t error)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>Extension</name>
    <title>Extension</title>
    <filename>group__Extension.html</filename>
    <member kind="function">
      <type>void *</type>
      <name>vector_get_ext_header</name>
      <anchorfile>group__Extension.html</anchorfile>
      <anchor>gafc2e34dbacffe6480af8681fbf46e808</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_ext_header_size</name>
      <anchorfile>group__Extension.html</anchorfile>
      <anchor>gacb4ec311fa5851f503419118b4984f54</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_data_offset</name>
      <anchorfile>group__Extension.html</anchorfile>
      <anchor>ga435ecb8dca435624d8d277feb6b2cc80</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>Properties</name>
    <title>Properties</title>
    <filename>group__Properties.html</filename>
    <member kind="function">
      <type>alloc_opts_t</type>
      <name>vector_alloc_opts</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>gafe6a5c3c08ddafc9be60051f447be0e1</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_element_size</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>gaae93d7f0c056c1d11d3232a7a7aa5e09</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_capacity</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>gab24bd801f91d4ae1ac79b54430b0409f</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>vector_capacity_bytes</name>
      <anchorfile>group__Properties.html</anchorfile>
      <anchor>ga3bfc1317eaeaee9b29a452c4fa41e4b3</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>Searches</name>
    <title>Searches</title>
    <filename>group__Searches.html</filename>
    <member kind="function">
      <type>void *</type>
      <name>vector_linear_find</name>
      <anchorfile>group__Searches.html</anchorfile>
      <anchor>ga184f04299040b2bfd8c03ef22abc7b8b</anchor>
      <arglist>(const vector_t *const vector, const size_t limit, const predicate_t predicate, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_binary_find</name>
      <anchorfile>group__Searches.html</anchorfile>
      <anchor>ga5081815905cf71570856aef717659463</anchor>
      <arglist>(const vector_t *const vector, const void *const value, const size_t limit, const compare_t cmp, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>ssize_t</type>
      <name>vector_binary_find_index</name>
      <anchorfile>group__Searches.html</anchorfile>
      <anchor>gaae3cfb2691a50da87169b5e057169782</anchor>
      <arglist>(const vector_t *const vector, const void *const value, const size_t limit, const compare_t cmp, void *const param)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>Elements</name>
    <title>Elements</title>
    <filename>group__Elements.html</filename>
    <member kind="function">
      <type>char *</type>
      <name>vector_data</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gac4d24b2cc8c8c44d36107b9a0fffea35</anchor>
      <arglist>(const vector_t *const vector)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_get</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga9437d9506a3b2799f595c88b82f160b3</anchor>
      <arglist>(const vector_t *const vector, const size_t index)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_set</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gab3a5e466261d31ee9edd77a5eb280030</anchor>
      <arglist>(vector_t *const vector, const size_t index, const void *const value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_set_zero</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gaf3eaea4a3a47741f46eae9ae6e984ef2</anchor>
      <arglist>(vector_t *const vector, const size_t index)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_spread</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga6312527827ba72441feeef4fc4047f94</anchor>
      <arglist>(vector_t *const vector, const size_t index, const size_t amount)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_shift</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga1f4cbce56499e81a048ac6454cd5bb69</anchor>
      <arglist>(vector_t *const vector, const size_t offset, const size_t length, const ssize_t shift)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_swap</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>gaf395f309bc15469998a8c14fc925b5b6</anchor>
      <arglist>(vector_t *const vector, const size_t index_a, const size_t index_b)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>vector_foreach</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga988880baa4fe32fb6d4299daf1a29a1f</anchor>
      <arglist>(const vector_t *const vector, const size_t limit, const foreach_t func, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>vector_aggregate</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga1a193e06f6700de896628631b8569740</anchor>
      <arglist>(const vector_t *const vector, const size_t limit, const aggregate_t func, void *const acc, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>vector_transform</name>
      <anchorfile>group__Elements.html</anchorfile>
      <anchor>ga176c007bd441e4469a28ba47ca952dfb</anchor>
      <arglist>(vector_t *const vector, const size_t limit, const transform_t func, void *const param)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>Allocation</name>
    <title>Allocation</title>
    <filename>group__Allocation.html</filename>
    <member kind="function">
      <type>void *</type>
      <name>vector_alloc</name>
      <anchorfile>group__Allocation.html</anchorfile>
      <anchor>gac06d4923e85171dfa1e3fae4f141b658</anchor>
      <arglist>(const size_t alloc_size, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>vector_realloc</name>
      <anchorfile>group__Allocation.html</anchorfile>
      <anchor>ga6bc2c0ee8e64283e06259c3edc6c6022</anchor>
      <arglist>(void *ptr, const size_t alloc_size, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>vector_free</name>
      <anchorfile>group__Allocation.html</anchorfile>
      <anchor>gacd97ea77de38db5fb50d9e8ba216a734</anchor>
      <arglist>(void *ptr, void *const param)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>Utilities</name>
    <title>Utilities</title>
    <filename>group__Utilities.html</filename>
    <member kind="function">
      <type>size_t</type>
      <name>calc_aligned_size</name>
      <anchorfile>group__Utilities.html</anchorfile>
      <anchor>gaacab7d4483d92f4ca41fbc548f5eab09</anchor>
      <arglist>(const size_t size, const size_t alignment)</arglist>
    </member>
    <member kind="function">
      <type>ssize_t</type>
      <name>cmp_lex_asc</name>
      <anchorfile>group__Utilities.html</anchorfile>
      <anchor>ga4b46f84c281c8dbe3e7e0e706d25053e</anchor>
      <arglist>(const void *const value, const void *const element, void *const param)</arglist>
    </member>
    <member kind="function">
      <type>ssize_t</type>
      <name>cmp_lex_dsc</name>
      <anchorfile>group__Utilities.html</anchorfile>
      <anchor>gab1d75d913a83778eb75c079544c4e4ae</anchor>
      <arglist>(const void *const value, const void *const element, void *const param)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>Guide</name>
    <title>📘 Guide</title>
    <filename>Guide.html</filename>
    <docanchor file="Guide.html" title="🔨 Creating a vector">create</docanchor>
    <docanchor file="Guide.html" title="⚙️ Default options">def_create</docanchor>
    <docanchor file="Guide.html" title="⚙️ Custom options">custom_opts</docanchor>
    <docanchor file="Guide.html" title="⚙️ No macro wrapper">no_macro_wrapper</docanchor>
    <docanchor file="Guide.html" title="⿻ Clone an existing vector">cloning</docanchor>
    <docanchor file="Guide.html" title="❗Error handling">error_resolution</docanchor>
    <docanchor file="Guide.html" title="🧨 Deallocating a vector">deallocate</docanchor>
    <docanchor file="Guide.html" title="⬆️ Extending a vector">extend</docanchor>
    <docanchor file="Guide.html" title="⚙️ Allocate extended header">extended_header</docanchor>
    <docanchor file="Guide.html" title="❗Customize resize error">resize_error</docanchor>
    <docanchor file="Guide.html" title="⚙️ Override default allocator">custom_allocator</docanchor>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Vector</title>
    <filename>index.html</filename>
    <docanchor file="index.html" title="Vector">md_README</docanchor>
  </compound>
</tagfile>
