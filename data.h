#ifndef DATA_H
#define DATA_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<ctype.h>
#include<pthread.h>
#include<stdio_ext.h>
#include<time.h>


#define SERVER_PORT 5501
#define SERVER_IP "172.29.8.180"
#define OKE_SIGNAL_LEN 10
#define BUFF_SIZE 4096

// enum ERROR_CODE{
//     ERROR_001, // Fail to connect to database
//     ERROR_002, // Fail to query database
//     ERROR_003
// };

const char ERROR[][10] = {
    "ERROR#000", // Fail to connect to database
    "ERROR#001", // Fail to query database
    "ERROR#002"
};

typedef struct {
    int opcode; // opcode = 999
    char end[4]; // end = "###"
} end_sig;

typedef struct {
    int opcode; // opcode = 100 
    char username[20];
    char password[20];
    int user_id;
} login_data;

typedef struct {
    int opcode; // opcode = 101
    char username[20];
    char password[20];
    char email[255];
} register_data;

typedef struct {
    int opcode; // opcode = 102
    int valid;
    int previlege;
    int user_id;
} login_server_response;

typedef struct{
    int opcode; // opcode = 201
    int exam_id;
    char title[255];
    int number_of_question;
    int admin_id;
}exam_data;

typedef struct{
    int opcode; // opcode = 202
    char content[1000];
    char op[4][300];
    char question_id[51];
}question_data;


typedef struct{
    char question_id[51];
    int ans;
} answer;

// typedef struct{
//     int opcode; // opcode = 207
//     int exam_id;
//     char username[30];
//     char score[16];
//     char date[100];
// } exam_result;

typedef struct{
    int opcode; // opcode = 207
    char date[255];
    char username[255];
    char title[256];
    char score[255];
} exam_result;

//!############## USER REQUEST #################

typedef struct{
    int opcode; // opcode = 103
    int user_id;
    char username[30];
} request_user_info;

typedef struct{
    int opcode; // opcode = 203
    int user_id;
    int number_of_exam;
}request_exam_list;

typedef struct{
    int opcode; // opcode = 204
    int user_id;
    int exam_id;
    int number_of_question;
}request_question_list;

typedef struct{
    int opcode; // opcode = 205
    int exam_id;
    char username[30];
    int user_id;
    int number_of_question;
    char submit_ans[300];
} submit_ans;  

typedef struct{
    int opcode; // opcode = 206
    int user_id;
    char username[30];
} request_exam_result;

typedef struct{
    int opcode; // opcode = 207
    int user_id;
    char username[30];
} request_user_history;

typedef struct{
    int opcode; // opcode = 301
    int user_id;
    char username[30];
} request_admin_privilege;

//!############## ADMIN REQUEST #################

//Search question request
typedef struct{
    int opcode; // opcode = 302
    int user_id;
    int next_page;
} check_admin_previlege_request;

typedef struct{
    int opcode; // opcode = 303
    int id;
    char username[30];
    char date[100];
} admin_request;

typedef struct{
    int opcode; // opcode = 304
    int user_id;
    char username[30];
} approve_admin_request;

typedef struct{
    int opcode; // opcode = 601
    int user_id;
    char question_id[51];
    int next_page;
} search_question_by_id;

typedef struct{
    int opcode; // opcode = 602
    int user_id;
    char question_content[1000];
    int next_page;
} search_question_by_content;

typedef struct{
    int opcode; // opcode = 603
    int user_id;
    char question_id[51];
    char question_content[1000];
    char topic[150];
    char subject[30];
    int next_page;
} advance_search_question;

typedef struct{
    int opcode; // opcode = 604
    int user_id;
    char question_id[51];
    char question_content[1000];
    char topic[150];
    char subject[30];
    char op[4][300];
    int ans;
} add_question;

typedef struct{
    int opcode; // opcode = 605
    int user_id;
    char question_id[51];
} delete_question;


// Exam function request

typedef struct{
    int opcode; //opcode = 701
    int user_id;
    int exam_id;
}search_exam_by_id;

typedef struct{
    int opcode; //opcode = 702
    int user_id;
    char title[255];
    int number_of_question;
    int admin_id;
}create_random_exam;

typedef struct{
    int opcode; //opcode = 703
    int user_id;
    char title[255];
    int number_of_ques;
}create_exam;

typedef struct{
    int opcode; //opcode = 704
    int user_id;
    char exam_id;
    int type; // 1 for add and 0 for delete
    int number_of_ques; //Number of question need to add or delete
}request_edit;


// ALL ABOUT ROOM

typedef struct{
    int r_id;
    char r_name[256];
    char admin_name[256];
    char create_date[16];
}room_info;

typedef struct{
    int opcode; //801 for create, 802 for delete
    char r_name[128];
    char username[256];
}room_create_del;

typedef struct{
    int opcode; // 803 for show created room, 804 for show added room , 805 for show all room
    char username[256]; 
}show_your_room;

typedef struct{
    int opcode; //811 for add student, 812 for remove
    int user_id; 
    int r_id;
}request_add_remove_student;

typedef struct{
    int opcode; // 821 for add test to the room;
    int test_id;
    int r_id;
}add_test_room;



// 810 - showListUserInRoom
// 811 - addUserToRoomByName

// 812 - deleteUserFromRoom

typedef struct{
    int opcode;
    char admin_name[256];
    int r_id;
    char user_name[256];
}room_user_add_del;

//!#########ADVANCED FEATURES############
typedef struct{
    int opcode; // 1001 taking exam with image embedded
    int user_id;
    int number_of_question;
} image_exam;

typedef struct{
    char question_id[51];
    char question_content[1000];
    char op[4][300];
    char image_path[255];
    int ans;
} question_image;

typedef struct{
    int opcode; // 1002 taking exam with sound embedded
    int user_id;
    int number_of_question;
} sound_exam;

typedef struct question_image question_sound;

typedef struct{
    int opcode; // 1003 see user study statistics
    int user_id;
    char username[30];
} user_stats;


//////////

char subject_list[][30] = {
    "Unknown", "Biochemistry", "Surgery", "Ophthalmology",
       "Physiology", "Gynaecology & Obstetrics", "Anaesthesia", "Psychiatry",
       "Microbiology", "Medicine", "Pharmacology", "Dental", "ENT",
       "Forensic Medicine", "Pediatrics", "Orthopaedics", "Radiology",
       "Pathology", "Skin", "Anatomy", "Social & Preventive Medicine"
};

char topic[][150] = {
    "Urinary tract", "Vitamins and Minerals",
       "Surgical Treatment Obesity", "Urology", "Bacteriology",
       "General anatomy", "general microbiology", "Biostatistics",
       "Embryology", "Testis & Scrotum", "Non communicable diseases",
       "Kidney", "Poisoning", "Facial Nerve And Its Disorders",
       "Spos Injury", "Miscellaneous (Obs)", "Mycology",
       "Anatomy of Bone & Fracture Healing", "Cerebrum", "Trauma",
       "Reversible Cell Injury",
       "Biosynthesis of Fatty Acids and Eicosanoids", "Other NHPs",
       "Concept of Health and Disease", "Communicable diseases",
       "Antiplatelets and Fibrinolytics", "Immunology", "Epidemiology",
       "General", "Congenital Dislocation of Hip (C.D.H.)",
       "Complications Of Anaesthesia", "Environment and Health", "G.I.T",
       "Head and neck",
       "Intra Uterine Growth Restriction, Intrapaum and Antepaum Fetal Surviellance",
       "Enterobecteriaceae", "Psychiatric Illness", "All India exam",
       "Myasthenia Gravis and Muscular dystrophy", "Anatomy",
       "Respiratory System", "Acute coronary syndrome",
       "General physiology", "Injuries", "Respiration", "Bacteria",
       "Abdominal Radiography", "Assessment of Vestibular Function",
       "Lower Limb Traumatology", "Sexual Offenses", "Breast", "vitamins",
       "Cardiovascular system", "Impoant Viral Diseases in Children",
       "Foot", "Influenza", "Immunity disorders",
       "Atherosclerosis and Arteriosclerosis", "C.V.S",
       "Respiratory chain", "Environment and health",
       "Bleeding Disorders", "Thyroid Gland", "Lower Extremity",
       "New born infants", "Intravenous Anesthetic Agents",
       "Cerebellum and Brainstem", "Cranial cavity",
       "Mullerian Abnormalities", "Mechanical injuries",
       "Metabolism of lipid", "Immunology and Rheumatology",
       "Substance Related and Addictive Disorders",
       "Medical Jurisprudence", "Thyroid",
       "Sexual offences and infanticide",
       "Characteristics and Epidemiology of Neoplasia",
       "Intestinal Infections, Worm Infestations", "Respiratory system",
       "Miscellaneous", "Central Nervous system",
       "Healthcare of the Community & International Health", "Neoplasia",
       "Thorax", "Pharynx",
       "Circulation: Circulatory Shock and Its Treatment",
       "Child psychiatry", "mycology", "Gynaecological oncology",
       "Muscles of arm and forearm region & Cubital fossa", "C.N.S.",
       "Pregnancy induced Hypeension", "Varicose Veins",
       "Gluconeogenesis", "Muscle relaxants", "Head and Neck Imaging",
       "Lens", "Asphyxia", "Cushing Syndrome",
       "VBDs, Arboviral & Viral Infections, Surface Infections",
       "Acute Kidney Injury", "miscellaneous", "Blood Vessels",
       "Molecular Biology", "Development period- week 1,2,3,4",
       "Testis and scrotum", "Nose and paranasal sinuses",
       "Nervous system", "JIPMER 2018", "JIPMER 2017", "Neuroanatomy",
       "Miscellaneous (Gynae)", "Ca Larynx", "Respiratory Failure",
       "Anticoagulants and Coagulants", "Endocrinology",
       "Layers of epidermis, dermis", "Sleep Disorders",
       "Preoperative assessment and monitoring in anaesthesia",
       "Dementia Due to Metabolic Causes",
       "Disorders of Ovulation - Anovulation ",
       "Raised ICP and Brain death", "Nutrition and Health",
       "Molecular Biology Techniques", "Metabolism of carbohydrate",
       "Concept of Control", "Hematology", "Apoptosis",
       "Nuclear medicine", "D.O.C", "Virology",
       "The Amnion and Umbilical Cord", "Spleen", "Fungal Infection",
       "General Pharmacology",
       "Law & Medicine, Identification, Autopsy & Burn", "Misc. (W.B.C)",
       "Respiratory",
       "Basic Concepts and Vascular changes of Acute Inflammation",
       "General pathology", "FMGE 2019", "Proteins and Amino Acids",
       "Enzymes", "Miscellaneous (Breast)", "Physiology & Histology",
       "Transcription", "Diagnosis in obstetrics", "Ear", "Nephrology",
       "Cardiac disorders", "Infection", "Management In Orthopedics",
       "Anxiety disorders stress disorders and grief",
       "Diseases of orbit, Lids and lacrimal apparatus", "Hearing Loss",
       "Substance Abuse", "Abdomen & Pelvis", "DNB 2018",
       "Disorders of the Respiratory Tract",
       "Sexually Transmitted Disease in the Female",
       "Disorders of Carbohydrate Metabolism", "Blood",
       "Aboions, Spontaneous & Induced Emergency Contraception (Hey,whats the hurry !)",
       "obstetrics,pediatrics and geriatrics", "Special topics",
       "Fluid, Electrolyte", "Liver", "Health Programmes in India",
       "Endocrinology and breast", "Other topics and Adverse effects",
       "Prostaglandins", "Inborn Errors of Metabolism", "General surgery",
       "Injuries Around Shoulder",
       "Schizophrenia Spectrum and Other Psychotic Disorders",
       "Thermal injury", "Hepatic Tumors, Cysts, and Abscesses",
       "Kidneys and Body Fluids",
       "Mycobacterial Diseases (TB, Leprosy and MAC)",
       "Special Senses: Hearing and Equilibrium", "Parasitology",
       "Diseases of Thyroid", "Muscle Physiology", "Neuroradiology",
       "Death and postmortem changes", "Anti-Malaria Drugs",
       "Pediatrics, environment and nutrition", "Lipids", "Bone Tumour",
       "Small & Large Intestine", "Cell Wall Synthesis Inhibitors",
       "Osteochondritis and Avascular Necrosis",
       "General Management of Wounds", "Anti-Parkinsonism", "G.I.T.",
       "Abdomen and pelvis", "Family Planning",
       "Symptoms and signs in psychiatry and classification",
       "Pectoral Region, Axilla & Brachial Plexus",
       "PH Care, Elements & Principles", "Stomach and Intestines",
       "Anesthesia for Cardiovascular Disease and Surgery", "Water",
       "Mycobacterial Infections", "TB of the Spine", "Nose and PNS",
       "Female Genital Tract", "Central Nervous System",
       "Penicillin & Cephalosporin", "Substance abuse",
       "Childhood tumors", "S.T.D.", "Fluid and electrolytes",
       "Targeted Anticancer Drugs and Immunosuppressants",
       "Intestinal obstruction", "Misc.", "Systemic histology",
       "Introduction to Forensic Medicine and Medical Jurisprudence",
       "General Considerations - Definition, Classification, Pathogenesis",
       "Anti Microbial", "Ultrasonography, CT, and MRI",
       "Anesthesia Circuit", "Infectious Lung Disease: Pneumonia",
       "Radiology Q Bank", "Stomach & Duodenum",
       "Community Ophthalmology", "Contraception",
       "Abdominal wall ,Inguinal and Femoral region",
       "Blistering disorders", "Organic Mental Disorder", "Chemotherapy",
       "Autonomic nervous system",
       "Ovarian Hyperstimulation Syndrome (OHSS)",
       "Epidemiological Study", "NEET 2019", "C.N.S", "C.V.S.",
       "Diabetes Mellitus", "Neuromuscular Blocker",
       "Mendelian Disorders: Single-Gene Defects",
       "Wounds, Tissue Repair & Scars", "Human identification",
       "Gastrointestinal tract", "Diseases of Cornea",
       "Immunology Pa 1 (Immune Response, Antigen-Antibody Reactions, Hypersensitivity, Structure of Immune System, Immunodeficiency Disorders)",
       "New Born Infants", "General Concepts", "Integrated QBank",
       "FMGE 2018", "Obstetrics", "COPD and Asthma", "General obstetrics",
       "Personality disorders", "Osteogenic Sarcoma", "ACE Inhibitors",
       "NEET Jan 2020", "Bacterial Infection of Skin", "Orbit",
       "Endocrine System", "Disorders in Menstruation",
       "Treatment in psychiatry", "Mental Health",
       "Imaging of Musculoskeletal Tumors",
       "Physiological Changes of Pregnancy",
       "Genetic and genetic disorders", "Fluid & Electrolyte",
       "Health care of community & international health",
       "Physiological changes during Pregnancy", "ENT Q Bank",
       "Lesions of the Visual Pathway", "Larynx",
       "Antimetabolites and Quinolones", "Diagnosis of Pregnancy",
       "Basics of DNA", "Health education & planning", "Autacoids",
       "Umblicial cord and diaphragm", "FMGE 2017", "Hepatitis",
       "Ocular Motility",
       "Forensic Toxicology - Concepts, Statutes, Evidence, and Techniques",
       "Secondary Glaucomas", "Miscellaneous (Bio-Chemistry)",
       "Neurotic Disorders",
       "Cellular Responses and Adaptations to Stress",
       "Pharmacotherapy in psychiatry", "Autoimmune skin disorders",
       "Psoriasis",
       "Systemic Bacteriology (Gram Positive Bacilli, Gram Negative Bacilli)",
       "Immune system", "Supracondylar Fracture", "Electrocardiography",
       "Sex Hormones", "Inflammation & Repair",
       "Newborn Infant - Physical Examination",
       "Cerebrovascular Diseases",
       "Systemic Bacteriology Pa 1 (Gram Positive Cocci, Gram Negative Cocci)",
       "Uveal Tract", "Medico legal procedures", "Anesthesia", "CT SCAN",
       "Haematology", "Metabolism of protein and amino acid",
       "Rectum and anal canal", "Irreversible cell Injury/Necrosis",
       "Psychoanalysis", "Symptoms & Signs ",
       "Neoplastic disorders of WBCs", "Respiratory System Pa 1",
       "Medical & Surgical Illness Complication Pregnancy",
       "Disorders of Gastrointestinal System Including Diarrhea",
       "Aplastic anemia", "Hepatitis Viruses", "Helminthology",
       "Neuroanatomy, Head and Neck", "Anaesthesia",
       "General Microbiology Pa 1 (History, Microscopy, Stains and Structure and Physiology of Bacteria)",
       "Miscellaneous General Anesthesia", "Errors of Refraction",
       "Concept of health and disease", "Arthritis", "Arterial Disorders",
       "Amyloidosis", "Tuberculosis", "Sensory System",
       "Disorders of Pubey", "Statistical Tests",
       "Death and Investigations",
       "Miscellaneous (Wounds, Tissue Repair & Scars)", "parasitology",
       "Mood Disorders", "Drugs", "Amino Acids Basics",
       "Miscellaneous (Neoplasia)", "Dystrophies and Degenerations",
       "Non Hodgkin Iymphoma", "Sexual offences",
       "Vitamins and Nutritional Deficiencies", "Upper limb",
       "JIPMER 2019", "Structure and function of protein",
       "Radial Nerve Injury", "Immunomodulator",
       "Histamine, 5-HT and PGs", "Communicable Diseases",
       "Nutrition and health", "Nervous System", "Ballistics",
       "Sleep disorders and eating disorders", "Tomour of Larynx",
       "Carbohydrates", "Head & Neck", "Gastrointestinal Radiology",
       "Toure methods", "Types of Errors of Refraction", "Hypolipidemic",
       "Cardiac and Pericardiac Imaging", "Cirrhosis",
       "Anti Adrenergic System", "Organic Mental Disorders", "Nutrition",
       "Prostate and seminal vesicles", "Nerves of Upper Extremity",
       "Cataract", "Indian Legal system, Legal sections & Cou procedures",
       "Anatomy of Inner Ear", "Gynaecological Anatomy",
       "Skeletal system", "Cell and Tissue Regeneration",
       "Dislocation of Elbow Joint", "Parasitology Pa 2 (Helminthology)",
       "Mood disorders", "Kidney and ureter", "Basic Concepts",
       "Normal Menstruation, Abnormal Menstruation, Menopausal Physiology and forsight of conception",
       "Specific Infections", "Pre Anesthetic Evaluation",
       "Endocrinology metabolism", "Disorders of nail",
       "Osteology of Thorax", "Autoimmune Skin Disorders",
       "General anaesthesia", "Cyanotic Congenital Heart Disease",
       "Growth and development", "Adrenal",
       "Virology Pa-2 (RNA Virus Pa-1,2 & Miscellaneous Viruses)",
       "PCOD, hirsutism and galactorrhea", "Radiotherapy Pa 1", "Alcohol",
       "Facial Nerve and its disorders", "Nutrition & Digestion",
       "Optics and Errors of Refraction", "Lukemia",
       "Regulation of Gene Expression ", "Fundamentals In Radiology",
       "Nasopharynx", "Cardiovascular System",
       "Clinical Aspects of Neoplasia", "Cholesterol and Lipoproteins",
       "Bladder Cancer", "Pancreas", "Metabolism of nucleic acids",
       "Lysosomal Storage Diseases", "Venous Disorders", "Special Senses",
       "Amino Acid Metabolic Disorder", "Upper Extremity",
       "Blood Vessels of Abdomen and Pelvis", "Gastro Intestinal System",
       "Cellular Pathology", "Diagnosis in Obstetrics", "Bile duct",
       "Renal physiology",
       "Investigation in ophthalmology and miscellaneous topics",
       "Brain ",
       "Labour - normal, abnormal, malposition, malpresentation and their management",
       "Preterm Labour", "Genetics", "Tumors", "Mutations & Codons",
       "Central Nervous System Tumors", "Vascular surgery",
       "Sexuality and intersexuality",
       "Thigh, Knee,Leg,Foot & Ankle injuries", "Thyroid Malignancies",
       "Inhalational Anesthetic Agents", "Esophagus", "Abdominal Injury",
       "Management and Long-Term Consequences",
       "Developmental-Behavioral Screening and Surveillance", "Oncology",
       "Disorders of menstruation", "Upper Limb Traumatology",
       "Other Key Definitions & Concepts", "Urogenital System",
       "Anti-Fungal Drugs", "Optics and refraction", "Pharyngeal Arches",
       "Colon and Rectum - Polyps and Carcinoma",
       "Parasitology Pa 1 (Protozoology)", "Anxiety & Stress",
       "Hospital waste and disaster management, Occupational health",
       "Congenital malformations", "Mechanics of Respiration",
       "Oesophagus", "Cytotoxic Anticancer Drugs", "Misc. (R.B.C)",
       "Data Variables", "Vitreous and retina", "Fungal infections",
       "Molecular Genetics", "Oral cavity & Oesophagus",
       "Aerial disorders", "Eye",
       "Bony dysplasia and soft tissue affection", "Electrolyte",
       "Disease of Infancy & Childhood",
       "Other FP Methods and New Initiatives in Family Planning",
       "Anesthesia for Liver Disease", "Respiratory Chain",
       "Stroke and TIA", "Neuroanatomy 3", "Lung Tumor",
       "Male Genital Tract", "ECG and Arrhythmias 1",
       "Neuro Ophthalmology", "Diphtheria", "Thorax And Mediastinum",
       "Growth, Development, and Behavior",
       "Lipid Oxidation and Synthesis", "Neuro-ophthalmology",
       "Blood Bank & Transfusion Therapy", "Infectious Disease",
       "Heart, Circulation, and Blood", "Gallbladder", "Urinary bladder",
       "General Microbiology (Sterilization and Bacterial Genetics)",
       "Breast Milk & Breast Feeding", "Forearm wrist and hand injuries",
       "Conducting System of Hea", "Pediatric dermatology", "Cornea",
       "Drowning", "Metabolic disorders - 1", "Lipoproteins", "Immunity",
       "Head Injury", "Fundamentals in Radiology",
       "Miscellaneous (E.N.T.)",
       "Colon, Rectum, and Anus - Diagnostic Evaluation", "Drug Abuse",
       "Avascular necrosis and osteochondroses",
       "Excretory System (Kidney, Bladder) Acid-Base Balance",
       "Platelet & Coagulation disorders",
       "Back, Deltoid, and Scapular Region",
       "Anatomy and physiology of bone and fracture healing",
       "Amino Acid Metabolism", "Multiple Myeloma",
       "Genetics And Genetic Disorders", "Seizures and Epilepsy",
       "Cailage tissue & cell junctions", "Anti-Helminths",
       "Social science, Mental health & Genetics", "Oral Cavity",
       "Miscellaneous (Neck)", "Hematopoeisis: Basic concepts",
       "Pathophysiology - Acute Pancreatitis", "Genito Urinary System",
       "Sexual Disorders", "Injuries of the Forearm",
       "Occupational Health", "Bacterial infection",
       "Anti-Parasitic Drugs", "Anatomy of skin", "Unusual Hernias",
       "Bordetella", "Urogynecology", "Small and Large intestine",
       "Bacterial infections", "Esophageal Motility Disorders",
       "Breast Cancer - Treatments", "A.N.S.",
       "Systemic Bacteriology (Haemophilus, Yersinia, Spirochaetes, Ricketssia, Chlamydia, Mycoplasma and Miscellaneous Bacteria)",
       "Endocrine", "Neoplasm of Mature B & T Cells", "Gonads",
       "Cardiac Output", "Disorders of hair",
       "Demography and family planning",
       "Iron Deficiency and Megaloblastic Anemia", "Conjunctiva",
       "Schizophrenia and other psychotic disorders",
       "Congenital Infections", "Nerve Injuries - 1",
       "Injuries Around the Fore Arm & Wrist", "Hernia",
       "Chemical Messengers: Receptors", "Intersex", "Joints",
       "Miscellaneous (Pharmacology)"
};

#endif