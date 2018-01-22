augroup LOCAL_SETUP
  "https://github.com/MarcWeber/vim-addon-local-vimrc
  nnoremap <leader>l :! build/singing_wind<CR>
  nnoremap <leader>b :wa <bar> :!cmake --build build<CR>

  let g:clang_format#style_options = {
    \ "BasedOnStyle": "Mozilla",
    \ "SortIncludes": "false"}
  let g:clang_format#auto_format = 1
  let g:clang_format#auto_format_on_insert_leave = 0
augroup end
