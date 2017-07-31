;*****************************************************************;
;*    NAME: Jacob Gerlach and Michael Benjamin                   *;
;*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     *;
;*    FILE: moos-mode.el                                         *;
;*    DATE: April 27th 2014                                      *;
;*                                                               *;
;* This file is part of MOOS-IvP                                 *;
;*                                                               *;
;* MOOS-IvP is free software: you can redistribute it and/or     *;
;* modify it under the terms of the GNU General Public License   *;
;* as published by the Free Software Foundation, either version  *;
;* 3 of the License, or (at your option) any later version.      *;
;*                                                               *;
;* MOOS-IvP is distributed in the hope that it will be useful,   *;
;* but WITHOUT ANY WARRANTY; without even the implied warranty   *;
;* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  *;
;* the GNU General Public License for more details.              *;
;*                                                               *;
;* You should have received a copy of the GNU General Public     *;
;* License along with MOOS-IvP.  If not, see                     *;
;* <http://www.gnu.org/licenses/>.                               *;
;*****************************************************************;

;;;;;;;Set up emacs 23 compatibility;;;;;;;;;;;;;;;;;;

;;This creates a fake prog-mode for moos to derive from. It's just
;;fundamental mode with the name changed, so you won't be able to use
;;the universal prog-mode hook, but it won't break moos mode.
(unless (fboundp 'prog-mode)
  (define-derived-mode prog-mode fundamental-mode "Prog"))

(unless (fboundp 'setq-local)
  ;;Macro definition copied from subr.el in Emacs 24 source
  (defmacro setq-local (var val)
    "Set variable VAR to value VAL in current buffer."
    (list 'set (list 'make-local-variable (list 'quote var)) val)))
;;;;;;;;;;;;;;;end of compatibility code;;;;;;;;;;;;;;

;;;###autoload
(define-derived-mode moos-mode prog-mode "MOOS"
  "Major mode for editing MOOS mission and behavior files"
  :syntax-table moos-mode-syntax-table
	;;Loading keywords is deferred until invoking the mode to support
	;;user content (see comment before moos-process-keywords), but we
	;;don't want to try to regenerate the keyword list every time we
	;;open a new moos file.
	(unless (boundp 'moos-font-lock-keywords)
					(moos-process-keywords))
	(setq-local font-lock-defaults '(moos-font-lock-keywords nil t))
  (setq-local comment-start "//")
  (setq-local indent-line-function 'moos-indent-line)
  (setq tab-width moos-default-tab-width)
	;;This is necessary to support app-specific keyword highlighting. If
	;;it ever starts causing problems, multiline font lock can also use
	;;font-lock-extend-after-change-region-function to further refine
	;;it's behavior. It hasn't been necessary so far.
  (setq-local font-lock-multiline 't)
	(use-local-map moos-mode-map)
	(run-hooks 'moos-mode-hook);;Major mode convention
)

;;Setup so that emacs auto-loads moos mode for .moos and .bhv files
;;According to the wiki, users should include this in .emacs rather
;;than putting it here. I don't think there's anybody out there who
;;wants to install this mode and edit moos files but doesn't want
;;this mode autoloading, so I'm ignoring this convention unless I get
;;a complaint. Makes it easier for the users if they don't need to
;;bother with this...  

;; \\' matches "end of string".
;; _? matches "zero or 1 underscores" so that the mission and behavior
;; files copied into Log folders by pLogger open in moos mode.
(add-to-list 'auto-mode-alist '("\\._?moos\\'" . moos-mode))
(add-to-list 'auto-mode-alist '("\\._?bhv\\'" . moos-mode))
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defvar moos-default-tab-width 2)

;;I found this online as a way of dealing with relative paths. Emacs
;;doesn't update the current directory when executing code. Since this
;;code is most likely executed by (require 'moos-mode) in the user's
;;init file, the working directory during execution is wherever that
;;init file is located (probaly the home directory). Since the app and
;;behavior files are wherever the user saved moos mode (probably not
;;the home directory), a relative path doesn't work even though
;;they're in the same folder as moos-mode.el. The "or" has to do with
;;how emacs arrived at this code, and I don't think both cases are
;;necessary to support the typical usage of loading emacs from an init
;;file, but I haven't taken the time to check this.

(load-file (concat (file-name-directory (or load-file-name buffer-file-name)) "moos-apps.el"))
(load-file (concat (file-name-directory (or load-file-name buffer-file-name)) "moos-bhvs.el"))

;;This function adds the app names to moos-app-names and adds entries
;;to moos-font-lock-keywords-1 that search for keywords anchored to
;;the app's processconfig entry. I should really define this inline as
;;a lambda function in the call to mapc
(defun moos-parse-app-and-keyword-list (input)
  (add-to-list 'moos-app-names (car input))
  (add-to-list 'moos-font-lock-keywords-1
	       `( 
		,(concat "ProcessConfig *= *" (car input))
		,(concat "^[ \t]*" (regexp-opt (cdr input)) "\\>")
		(moos-find-end-of-config)
		nil
		(0 font-lock-keyword-face))))

;;Same as above except for behaviors. Should also be inline as a
;;lambda in mapc below.
(defun moos-parse-bhv-and-keyword-list (input)
  (add-to-list 'moos-bhv-names (car input))
  (add-to-list 'moos-font-lock-keywords-1
	       `( 
		,(concat "Behavior *= *" (car input))
		,(concat "^[ \t]*" (regexp-opt (cdr input)) "\\>")
		(moos-find-end-of-config)
		nil
		(0 font-lock-keyword-face))))

;;These two functions allow users to add their own local MOOS apps and
;;behaviors. These should be called in the users init file, so that
;;the appropriate name and keyword list is modified before generating
;;the font lock constructs when moos-mode is called to handle a moos
;;file.
(defun moos-add-user-app (input)
	"Add a user defined MOOS app name and keywords to font lock list.

   INPUT should be a list of strings where the first element is
   the app name and the remaining elements are the associated
   keywords.

   For example:

   (moos-add-user-app '(\"pFoobar\" \"foo\" \"bar\" \"baz\"))"
	(add-to-list 'moos-app-and-keyword-list input))

(defun moos-add-user-bhv (input)
	"Add a user defined MOOS behavior name and keywords to font lock list.

   INPUT should be a list of strings where the first element is
   the behavior name and the remaining elements are the
   associated keywords.

   For example:

   (moos-add-user-bhv '(\"BHV_Foobar\" \"foo\" \"bar\" \"baz\"))"

	(add-to-list 'moos-app-and-keyword-list input))

;;All keyword processing is wrapped in this function. This allows
;;processing to be deferred until the mode is loaded, which ensures
;;that any calls to moos-add-user-bhv or moos-add-user-app in the
;;user's init file have been completed.
(defun moos-process-keywords ()
	(defconst moos-font-lock-keywords-1
		(list
		 ;;nsplug commands and macros
		 ;;\\$\(.*\) matches anything wrapped in $( )
		 '("\\$\(.*\)\\|#include\\|#ifdef\\|#elseifdef\\|#endif\\|#else"
			 . font-lock-preprocessor-face)

		 ;;A word after #ifdef or #elseifdef is a preprocessor variable
		 '("#ifdef\\|#elseifdef" "\\<[A-Z_\-]*\\>" 
			 nil nil (0 font-lock-preprocessor-face))

		 ;;Generic mission file keywords. These are either common to all
		 ;;apps or outside of the processConfig blocks.
		 '("\\<AppTick\\|CommsTick\\|Community\\|ProcessConfig\\|ServerHost\\|ServerPort\\|LatOrigin\\|LongOrigin\\|MOOSTimeWarp\\|MaxAppTick\\|IterateMode\\>"
			 . font-lock-keyword-face)

		 ;;Generic behavior keywords. These are defined on the superclass
		 ;;and valid for all behaviors. If these end up getting highlighted
		 ;;in other places, make this into an anchored matcher using
		 ;;Behavior =
		 '("\\<\\(?:initialize\\|set\\|name\\|pwt\\|duration\\|duration_idle_decay\\|duration_status\\|duration_reset\\|condition\\|updates\\|perpetual\\|endflag\\|runflag\\|idleflag\\|activeflag\\|inactiveflag\\|templating\\)\\>"
			 . font-lock-keyword-face)

		 '("\\<true\\|false\\>"
			 . font-lock-constant-face))
		"Minimal highlighting expressions for MOOS mode")

	;;These lists get populated in the call to parse-xx-and-keyword-list
	;;below. They need to exist so add-to-list can do its thing...
	(setq moos-app-names '("MOOSDB"))
	;;(setq moos-bhv-names '("FOO"))  ;; mikerb

	;;Iterate through the app and behavior lists to add the font-lock
	;;entries. These lists were created in the separate files
	;;moos-apps.el and moos-bhvs.el and potentially modified by any user
	;;calls to moos-add-user-app or -bhv
	(mapc 'moos-parse-app-and-keyword-list moos-app-and-keyword-list)
	(mapc 'moos-parse-bhv-and-keyword-list moos-bhv-and-keyword-list)

	;;Collecting the app and behavior names in one list and then adding
	;;them as below does two things: (1), it allows the entire list of
	;;names to be searched as one regular expression, improving
	;;efficiency, and (2) The structure of moos-font-lock-keywords is
	;;less fragmented. Instead of alternating keywords/name as each app
	;;or behavior is read from it's list. This is important since
	;;font-lock is first-come first-served.

	;;Add the collected app names for fontification
	(add-to-list 'moos-font-lock-keywords-1
							 (cons (regexp-opt moos-app-names 'words)
										 font-lock-type-face))

	;;Add the collected bhv names for fontification
	(add-to-list 'moos-font-lock-keywords-1
							 (cons (regexp-opt moos-bhv-names 'words)
										 font-lock-type-face))

	;;The defconst and defvar sequence comes from an emacs major mode
	;;tutorial that I read. In theory, the font-lock keywords list would
	;;be built in levels. moos-font-lock-keywords-2 (another constant)
	;;would append additional keywords to keywords-1, and keywords-3
	;;would append yet more keywords. Then by assigning the appropriate
	;;list to the variable moos-font-lock-keywords, different levels of
	;;fontification could be achieved. This is actually built in to
	;;emacs font-lock handling using the variable
	;;font-lock-maximum-decoration. I don't know if max-decoration would
	;;automatically pick up the layered lists, or if additional work
	;;would be necessary to make them work together. I don't intend on
	;;developing this feature unless someone asks for it.
	(defvar moos-font-lock-keywords moos-font-lock-keywords-1
		"Default highlighting expressions for MOOS mode"))

;;Set up  Syntax Table
(defvar moos-mode-syntax-table
  (let ((st (make-syntax-table)))
    ;;A backslash can start(1) or be the second char(2) in a comment
    ;;A comment start ended with a slash (2) is a b style comment
    (modify-syntax-entry ?/ ". 12b" st)
    ;;A newline ends a b style comment
    (modify-syntax-entry ?\n "> b" st)
    ;;Underscores and dashes are valid parts of words. This is
    ;;necessary to prevent font-locking false in foobar_false or set
    ;;in to-be-set
    (modify-syntax-entry ?_ "w" st)
    (modify-syntax-entry ?- "w" st)
    st)
    "Syntax table for moos-mode")

;;This indent function is extremely basic and has plenty of room for
;;improvement. In particular, it does not currently handle the
;;customary format for hierarchical mode declarations in behavior
;;files.
(defun moos-indent-line ()
  "Indent current line as moos-code"
  (interactive)
  (let (new-indent)
    (save-excursion
      (beginning-of-line)
      (if (bobp)
	  (setq new-indent 0)
	(forward-line -1)

	;;This is perhaps overkill. Are there any places in moos code with
	;;nested constructs that increase indentation? Simpler would be to
	;;indent anything that's not a brace or ProcessConfig/Behavior to
	;;tab-width.
	(if (eq(current-indentation) 0)
	    (setq new-indent (+ (current-indentation) tab-width))
	  (setq new-indent (current-indentation)))))
    ;;It's possible to trap this in case new-indent was never set, but
    ;;I don't think that can happen with the way I've written the
    ;;indent function
    (indent-line-to new-indent)))
  

;;TODO: Add a link to the font lock documentation that explains what a
;;pre-form is
(defun moos-find-end-of-config ()
  "pre-form for anchored font lock. Returns buffer position of end of
   current configuration block.

   This function is used to enable multiline anchored font lock
   constructs. The anchor is the ProcessConfig or Behavior = line, but
   font-lock needs to know where to stop looking relative to that
   anchor."
  (save-excursion
    ;;processconfig and behavior are included here so that a block
    ;;that's missing a close brace doesn't break font-lock for the
    ;;rest of the file. An alternative approach would be to just look
    ;;for the closing brace, so that the change in fontification after
    ;;the missing brace would draw attention to the error.
    (or 
		 (re-search-forward "^[ \t]*\}\\|processconfig\\|behavior =" nil t) 
		 (point-max))
    (point)))


;;Idea for extending this function: provide option to make it not
;;respect existing whitespace between "variable" and "=", so that 
;;"foo  = bar" or "foo=bar", when justified, both become "foo = bar". 
;;Even better, control this behavior by use of a prefix argument where
;;N is the number of spaces between the variable and the equals sign
;;(default 1)
(defun moos-justify-equals (start end)
  "Indents current region to justify equals signs"
  (interactive "r")
  (save-excursion
		(let ((left-length 0))
			;;Since this function will modify the number of characters in
			;;the region (and therefore the buffer position of the end of
			;;the region, it has to iterate through the region back to
			;;front, so that the start position, which will not change, can
			;;be used as a reference.
			(goto-char end)
			(goto-char (line-beginning-position))
			(save-excursion
				(while (>= (point) start)
					;;This regular expression is "any amount of whitespace made
					;;up of tabs or spaces, followed by a group of any
					;;characters and then an equals sign. The repeater in the
					;;group before the equals sign is non hungry (*?) because
					;;otherwise lines such as "points = lawnmower = ..." get
					;;justified to the second equals sign (after lawnmower)
					;;instead of the first one
					(if (re-search-forward "[ \t]*\\(.*?\\)=" (line-end-position) t)
							(when (> (length (match-string-no-properties 1)) left-length)
								(setq left-length (length (match-string-no-properties 1)))))
					(forward-line -1)))
			;;The innner save-excursion was closed, so point is back at the
			;;end of the region, iteration will be back to start again
			(while (>= (point) start)
				(if (re-search-forward "^[ \t]*\\(.*?\\)=" (line-end-position) t)
						;;For the longest "left side" identified above, the
						;;subtraction will evaluate to zero and that line will
						;;just be indented to tab-width. All other lines will
						;;receive extra white space such that the equals signs are
						;;aligned.
						(indent-line-to (+ tab-width (- left-length (length (match-string-no-properties 1)))))
					;;Use default indentation on lines that aren't
					;;variable-value pairs. Except that if the start or end of
					;;region is at the start or end of the configuration block,
					;;we don't want to indent the brace.
					(unless (looking-at "[{}]")
						(indent-line-to tab-width)))
				(forward-line -1)))))

(defvar moos-mode-map nil "Keymap for moos-mode")
(setq moos-mode-map (make-sparse-keymap))
(define-key moos-mode-map (kbd "C-=") 'moos-justify-equals)

;;In accordance with standard emacs major mode conventions, this hook
;;is run as the last step of invoking moos-mode, which allows users to
;;use the hook in their init file to automate moos-specific
;;customizations of their choice
(defvar moos-mode-hook nil)

(provide 'moos-mode)
