;;; File: emacs-format-file
;;; original Stan Warford
;;; 17 May 2006
;;; modified t. schneider
;;; 1.9.08

(defun emacs-format-function ()
   "Format the whole buffer."   
   (c-set-style "my-c-style")
   (c-set-offset 'substatement-open '0) ; brackets should be at same indentation level as the statements they open
   (c-set-offset 'inline-open '+)
   (c-set-offset 'block-open '+)
   (c-set-offset 'brace-list-open '+) ; all "opens" should be indented by the c-indent-level
   (c-set-offset 'case-label '+) ; indent case labels by c-indent-level, too
   (goto-char (point-min))
   (while (search-forward "^I" nil t) (replace-match " " nil t))
   (indent-region (point-min) (point-max) nil)
   (untabify (point-min) (point-max))
   (save-buffer)
)
