import os
import re
import pandas as pd
from tkinter import Tk, filedialog
from datetime import datetime
import subprocess
 
# Function to get user-selected Excel file
def get_excel_file():
    root = Tk()
    root.withdraw()  # Hide the main Tkinter window
    file_path = filedialog.askopenfilename(
        title="Select Excel File",
        filetypes=[("Excel Files", "*.xlsx *.xls")]
    )
    return file_path
 
# Load replacement rules from Excel file
def load_replacement_rules(file_path):
    df = pd.read_excel(file_path)
    replacement_rules = dict(zip(df['old_name'], df['new_name']))
    return replacement_rules
 
# Function to rename files based on replacement rules and log renaming actions
def rename_files(folder_path, replacement_rules):
    # Create a log file directory if it doesn't exist
    log_dir = "C:\\Customization tool\\rename files"
    os.makedirs(log_dir, exist_ok=True)
 
    # Create a log file with a timestamp
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    log_file_name = f"filter_{timestamp}.txt"
    log_file_path = os.path.join(log_dir, log_file_name)
 
    with open(log_file_path, "w") as log_file:
        for root, dirs, files in os.walk(folder_path):
            for file_name in files:
                new_name = file_name
                for old_pattern, new_value in replacement_rules.items():
                    new_name = re.sub(old_pattern, new_value, new_name)
                if new_name != file_name:
                    old_file_path = os.path.join(root, file_name)
                    new_file_path = os.path.join(root, new_name)
                    try:
                        # Rename the file
                        os.rename(old_file_path, new_file_path)
                        log_file.write(f"Renamed: {file_name} -> {new_name}\n")
                        print(f"Renamed: {file_name} -> {new_name}")
                    except FileExistsError:
                        print(f"Error: File already exists: {new_file_path}")
                        log_file.write(f"Error: File already exists: {new_file_path}\n")
 
    # Open the folder and log file after renaming
    print(f"Renaming complete. Log saved at: {log_file_path}")
    os.startfile(folder_path)
    os.startfile(log_file_path)
 
# Main script
if __name__ == "__main__":
    # Get folder path from user input
    folder_path = input("Give the folder path: ").strip()
    if not os.path.isdir(folder_path):
        print("Invalid folder path. Please check and try again.")
    else:
        # Get user-selected Excel file
        excel_file = get_excel_file()
        if excel_file:
            replacement_rules = load_replacement_rules(excel_file)
            rename_files(folder_path, replacement_rules)