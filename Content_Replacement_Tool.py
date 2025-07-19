import pandas as pd
import re
import tkinter as tk
from tkinter import filedialog
from tkinter import messagebox

def read_replacements_from_excel(excel_file_path, sheet_name='Sheet1'):
    try:
        df = pd.read_excel(excel_file_path, sheet_name=sheet_name)
        df['old_content'] = df['old_content'].fillna('')
        df['new_content'] = df['new_content'].fillna('')
        replacements = {
            str(row['old_content']).strip(): str(row['new_content']).strip()
            for _, row in df.iterrows()
            if str(row['old_content']).strip()
        }
        print("Replacements read from Excel:", replacements)
        return replacements
    except Exception as e:
        messagebox.showerror("Error", f"An error occurred: {str(e)}")
        print(f"An error occurred: {e}")
        return None

def replace_multiple_contents_in_cpp_file(file_path, replacements):
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            lines = file.readlines()
        
        new_lines = []
        for line in lines:
            skip_line = False
            line_stripped = line.strip()
            for old_content, new_content in replacements.items():
                if old_content and old_content in line_stripped:
                    if new_content:
                        line = re.sub(re.escape(old_content), new_content, line)
                    else:
                        skip_line = True
                        break
            if not skip_line:
                new_lines.append(line)
        
        new_content = ''.join(new_lines)
        with open(file_path, 'w', encoding='utf-8') as file:
            file.write(new_content)
        messagebox.showinfo("Success", "Successfully replaced the content.")
    except Exception as e:
        messagebox.showerror("Error", f"An error occurred: {str(e)}")
        print(f"An error occurred: {e}")

def main():
    root = tk.Tk()
    root.withdraw()
    excel_file_path = filedialog.askopenfilename(
        title="Select the Excel file",
        filetypes=[("Excel files", "*.xlsx;*.xls")]
    )
    if not excel_file_path:
        print("No Excel file selected. Exiting...")
        return
    
    cpp_file_path = filedialog.askopenfilenames(
        title="Select the C++ and/or Header files",
        #filetypes=[("C++ and Header files", "*.cpp;*.h")]
        filetypes=[("C++ and Header files", "*.*")]
    )
    if not cpp_file_path:
        print("No C++ file selected. Exiting...")
        return
    
    replacements = read_replacements_from_excel(excel_file_path)
    if replacements:
        for file_path in cpp_file_path:
            replace_multiple_contents_in_cpp_file(file_path, replacements)

if __name__ == "__main__":
    main()
