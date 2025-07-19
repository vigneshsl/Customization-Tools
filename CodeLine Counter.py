import os

def count_lines_in_file(file_path):
    if not os.path.isfile(file_path):
        return 0

    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()
        code_lines = [line for line in lines if line.strip() and not line.strip().startswith(("#", "//", "/*", "*", "*/"))]
    
    return len(code_lines)

def process_folder(folder_path):
    if not os.path.isdir(folder_path):
        print("Error: Folder does not exist.")
        return
    
    header_files = []
    cpp_files = []
    total_loc_header = 0
    total_loc_cpp = 0

    # Scan all files in the given folder
    for root, _, files in os.walk(folder_path):
        for file in files:
            file_path = os.path.join(root, file)

            if file.endswith(".h"):
                loc = count_lines_in_file(file_path)
                total_loc_header += loc
                header_files.append((file, loc))
            elif file.endswith(".cpp"):
                loc = count_lines_in_file(file_path)
                total_loc_cpp += loc
                cpp_files.append((file, loc))

    # Convert to KLOC
    kloc_header = total_loc_header / 1000
    kloc_cpp = total_loc_cpp / 1000

    # Beginner-friendly LOC Calculation Explanation
    print("\n==============================")
    print("      LOC & KLOC CALCULATION")
    print("==============================\n")

    print("LOC (Lines of Code) counts the actual number of meaningful code lines in a file.")
    print("It ignores empty lines and comments (e.g., // in C++).")
    print("\nFormula for LOC Calculation:")
    print("  LOC = Total meaningful lines in the file\n")

    print("KLOC (Kilo Lines of Code) represents LOC in thousands.")
    print("\nFormula for KLOC Calculation:")
    print("  KLOC = LOC / 1000\n")

    # Print tabular results
    print("\nHeader Files (LOC Count):")
    print(f"{'File Name':<30}{'LOC':>10}")
    print("-" * 40)
    for file, loc in header_files:
        print(f"{file:<30}{loc:>10}")

    print("\nSource Files (LOC Count):")
    print(f"{'File Name':<30}{'LOC':>10}")
    print("-" * 40)
    for file, loc in cpp_files:
        print(f"{file:<30}{loc:>10}")

    print("\n\n\n==============================")
    print("         FINAL SUMMARY")
    print("==============================\n")

    print(f"Total LOC for Header Files: {total_loc_header}")
    print(f"KLOC Calculation: KLOC = {total_loc_header} / 1000 = {kloc_header:.2f}\n")

    print(f"Total LOC for Source Files: {total_loc_cpp}")
    print(f"KLOC Calculation: KLOC = {total_loc_cpp} / 1000 = {kloc_cpp:.2f}")
def main(): 
    folder_path = input("Enter the folder path: ").strip()
    process_folder(folder_path)
 

if __name__ == "__main__":
    main()
 