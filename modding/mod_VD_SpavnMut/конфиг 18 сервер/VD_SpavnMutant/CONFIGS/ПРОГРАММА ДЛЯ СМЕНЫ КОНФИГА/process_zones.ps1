






############## ИНФОРМАЦИЯ!!!!!!!!!!!!!!!! ##############
##############  Для файла zombie_territories.xml берётся значение dmax из XML: ##############

############## Для остальных файлов (ModMutants.xml, VANILLA_ANIMALS...) берётся smax. ##############







# Пути к входным XML-файлам
$inputFiles = @("ModMutants.xml", "zombie_territories.xml", "VANILLA_ANIMALS_BY_VD_SPAWNER.xml")

# Имена выходных файлов
$outputFiles = @(
    "Dnevnoi.txt", "Nochnoy.txt"
)

# Очистка всех выходных файлов (если они существуют)
foreach ($file in $outputFiles) {
    if (Test-Path $file) { Clear-Content -Path $file }
}

# Словарь возможных замен для класснеймов
$classReplacements = @{
    "KARP_Mutant_Krovos"     = @("KARP_Mutant_Krovosos1", "KARP_Mutant_Krovosos2", "KARP_Mutant_Krovosos3")
    "KARP_Mutant_Izlom"      = @("KARP_Mutant_Izlom", "KARP_Mutant_Izlom_kletka")
    "KARP_Mutant_Plot"       = @("KARP_Mutant_Plot_Grey", "KARP_Mutant_Plot_Green")
    "KARP_Mutant_Blinddog"   = @("KARP_Mutant_Blinddog1", "KARP_Mutant_Blinddog2")
    "SFP_Zombirovan"         = @("SFP_Zombirovan_ondinocka", "SFP_Zombirovan_DOLG", "SFP_Zombirovan_Neitral",
                                 "SFP_Zombirovan_Free", "SFP_Zombirovan_Monolit", "SFP_Zombirovan_Naem")
    "SFP_ZombieCivilian"     = @("SFP_ZombieCivilian_1", "SFP_ZombieCivilian_2", "SFP_ZombieCivilian_3",
                                 "SFP_ZombieCivilian_4", "SFP_ZombieCivilian_5", "SFP_ZombieCivilian_6",
                                 "SFP_ZombieCivilian_7", "SFP_ZombieCivilian_8", "SFP_ZombieCivilian_9")
    "MutCatBayun"            = @("SFP_CatBaun", "KARP_Mutant_Cat")
	
	"SFP_Dikoy"              = @("SFP_Dikoy_BrodagaHands", "SFP_Dikoy_BM6Knife", "SFP_Dikoy_NoviceAxe", "SFP_Dikoy_MilitaryHands", "SFP_Dikoy_PlashKosa", 
								 "SFP_Dikoy_BM6Hands", "SFP_Dikoy_BanditWhite00Axe", "SFP_Dikoy_GorkaKnife", 
								 "SFP_Dikoy_UssRBanditKnife", "SFP_Dikoy_NoviceHands", "SFP_Dikoy_GrehKnife", "SFP_Dikoy_BrodagaAxe")
	
	"SFP_MozgoTrasbl"         = @("SFP_MozgoTras", "SFP_MozgoTras_red")
	"SFP_Piyavkbl"            = @("SFP_Piyavka", "SFP_Piyavka_invz")
	"SFP_Shipastiybl"		  = @("SFP_Shipastiy", "SFP_Shipastiy_invz")
	
	"Era_Zombies"         	  = @("Era_Zombies_MumiaSlowZmb_HARD", "Era_Zombies_MurderZmb_HARD", "Era_Zombies_Zombie_HARD", "Era_Zombies_Zubastik1_HARD", "Era_Zombies_Zubastik2_HARD", "Era_Zombies_Zubastik3_HARD")

	"MSFZ_Mutant_Entbl"       = @("MSFZ_Mutant_Ent", "MSFZ_Mutant_Ent_Medium", "MSFZ_Mutant_Ent_Hard")
	
	"MSFZ_Mutant_Spiderbl"    = @("MSFZ_Mutant_Spider1", "MSFZ_Mutant_Spider2", "MSFZ_Mutant_Spider3")
	
	"MSFZ_Mutant_Zanozbl"     = @("MSFZ_Mutant_Zanoza1", "MSFZ_Mutant_Zanoza2", "MSFZ_Mutant_Zanoza3")
	
	"Troll"     			  = @("MSFZ_Troll")
	
	"Ricar"     			  = @("MSFZ_Ricar")
	
	# Рандомизируем ванильных животных
	"Animal_Zayac"				= @("Animal_LepusEuropaeus")
	"Animal_Lisa"            	= @("Animal_VulpesVulpes")
	"Animal_Kosulya"            = @("Animal_CapreolusCapreolus", "Animal_CapreolusCapreolusF")
	"Animal_Olen"            	= @("Animal_CervusElaphus", "Animal_CervusElaphusF")
	"Animal_Kozel"            	= @("Animal_CapraHircusF", "Animal_CapraHircus_Black", "Animal_CapraHircus_Brown", "Animal_CapraHircus_White")
	"Animal_Kaban"            	= @("Animal_SusScrofa")
	"Animal_OvcaBaran"         	= @("Animal_OvisAries", "Animal_OvisAriesF")
	"Animal_Medved"            	= @("Animal_UrsusArctos")
	"Animal_PetuhKurica"        = @("Animal_GallusGallusDomesticus", "Animal_GallusGallusDomesticusF_Brown", "Animal_GallusGallusDomesticusF_Spotted", "Animal_GallusGallusDomesticusF_White")
	"Animal_Korovi"            	= @("Animal_BosTaurusF_Brown", "Animal_BosTaurusF_Spotted", "Animal_BosTaurusF_White", "Animal_BosTaurus_Brown", "Animal_BosTaurus_Spotted", "Animal_BosTaurus_White")
	"Animal_Svinya"            	= @("Animal_SusDomesticus")
	"Animal_Volki"				= @("Animal_CanisLupus_Grey", "Animal_CanisLupus_White")
	"Animal_Oleni_Winter"		= @("Animal_RangiferTarandusF", "Animal_RangiferTarandus")

	# Рандомизируем МОДОВЫХ ВАНИЛЬНЫХ животных(Например медведи с намальска)
	"Animal_PolarMedved"		= @("Animal_UrsusMaritimus")	
	
	
    # Добавляем замены для классов с ванильными зомби
	"InfectedPrisoner"       = @("ZmbM_PrisonerSkinny")
	"InfectedCity"           = @("ZmbF_CitizenANormal_Brown", "ZmbF_Clerk_Normal_Green", "ZmbF_JournalistNormal_White", "ZmbF_ShortSkirt_black", "ZmbF_ShortSkirt_red", "ZmbF_ShortSkirt_stripes", "ZmbF_SurvivorNormal_Orange", "ZmbM_CitizenASkinny_Brown", "ZmbM_ClerkFat_Brown", "ZmbM_CommercialPilotOld_Grey", "ZmbM_MotobikerFat_Black", "ZmbM_SkaterYoung_Blue", "ZmbF_BlueCollarFat_White", "ZmbF_CitizenANormal_Beige", "ZmbF_CitizenANormal_Blue", "ZmbF_CitizenBSkinny", "ZmbF_Clerk_Normal_Blue", "ZmbF_Clerk_Normal_Red", "ZmbF_Clerk_Normal_White", "ZmbF_JournalistNormal_Blue", "ZmbF_JournalistNormal_Green", "ZmbF_JournalistNormal_Red", "ZmbF_ShortSkirt_beige", "ZmbF_ShortSkirt_brown", "ZmbF_ShortSkirt_checks", "ZmbF_ShortSkirt_green", "ZmbF_ShortSkirt_grey", "ZmbF_ShortSkirt_white", "ZmbF_ShortSkirt_yellow", "ZmbF_SkaterYoung_Brown", "ZmbF_SkaterYoung_Striped", "ZmbF_SkaterYoung_Violet", "ZmbF_SurvivorNormal_Blue", "ZmbF_SurvivorNormal_Red", "ZmbF_SurvivorNormal_White", "ZmbM_CitizenASkinny_Blue", "ZmbM_CitizenASkinny_Grey", "ZmbM_CitizenASkinny_Red", "ZmbM_CitizenBFat_Blue", "ZmbM_CitizenBFat_Green", "ZmbM_CitizenBFat_Red", "ZmbM_ClerkFat_Grey", "ZmbM_ClerkFat_Khaki", "ZmbM_ClerkFat_White", "ZmbM_CommercialPilotOld_Blue", "ZmbM_CommercialPilotOld_Brown", "ZmbM_CommercialPilotOld_Olive", "ZmbM_Gamedev_Black", "ZmbM_Gamedev_Blue", "ZmbM_Gamedev_Gray", "ZmbM_JournalistSkinny", "ZmbM_MotobikerFat_Beige", "ZmbM_SkaterYoung_Brown", "ZmbM_SkaterYoung_Green", "ZmbM_SkaterYoung_Grey")
	"InfectedReligious"      = @("ZmbM_priestPopSkinny")
	"InfectedSolitude"       = @("ZmbF_HikerSkinny_Blue", "ZmbF_HikerSkinny_Green", "ZmbF_HikerSkinny_Grey", "ZmbF_HikerSkinny_Red", "ZmbM_FishermanOld_Blue", "ZmbM_FishermanOld_Green", "ZmbM_FishermanOld_Grey", "ZmbM_FishermanOld_Red", "ZmbM_HermitSkinny_Beige", "ZmbM_HermitSkinny_Black", "ZmbM_HermitSkinny_Green", "ZmbM_HermitSkinny_Red", "ZmbM_HikerSkinny_Blue", "ZmbM_HikerSkinny_Green", "ZmbM_HikerSkinny_Yellow", "ZmbM_HunterOld_Autumn", "ZmbM_HunterOld_Spring", "ZmbM_HunterOld_Summer", "ZmbM_HunterOld_Winter")
	"InfectedVillage"        = @("ZmbM_SkaterYoung_Brown", "ZmbM_SkaterYoung_Green", "ZmbM_SkaterYoung_Grey", "ZmbM_SkaterYoung_Blue", "ZmbF_JoggerSkinny_Blue", "ZmbF_JoggerSkinny_Brown", "ZmbF_JoggerSkinny_Green", "ZmbF_MilkMaidOld_Beige", "ZmbF_MilkMaidOld_Black", "ZmbF_MilkMaidOld_Grey", "ZmbF_VillagerOld_Green", "ZmbF_VillagerOld_Red", "ZmbF_VillagerOld_White", "ZmbM_FarmerFat_Blue", "ZmbM_FarmerFat_Brown", "ZmbM_FarmerFat_Green", "ZmbM_Jacket_beige", "ZmbM_Jacket_black", "ZmbM_Jacket_blue", "ZmbM_Jacket_brown", "ZmbM_Jacket_greenchecks", "ZmbM_Jacket_grey", "ZmbM_Jacket_khaki", "ZmbM_Jacket_stripes", "ZmbM_JoggerSkinny_Blue", "ZmbM_JoggerSkinny_Green", "ZmbM_VillagerOld_Blue", "ZmbM_VillagerOld_White", "ZmbF_JoggerSkinny_Red", "ZmbF_MilkMaidOld_Green", "ZmbF_VillagerOld_Blue", "ZmbM_FarmerFat_Beige", "ZmbM_Jacket_bluechecks", "ZmbM_Jacket_magenta", "ZmbM_JoggerSkinny_Red", "ZmbM_VillagerOld_Green", "ZmbF_NurseFat_Winter", "ZmbF_PatientOld_Winter", "ZmbM_PatientSkinny_Winter")
	"InfectedArmy"           = @("ZmbM_usSoldier_Heavy_Woodland", "ZmbM_usSoldier_Officer_Desert", "ZmbM_PatrolNormal_Autumn", "ZmbM_PatrolNormal_Flat", "ZmbM_PatrolNormal_PautRev", "ZmbM_PatrolNormal_Summer", "ZmbM_SoldierNormal", "ZmbM_usSoldier_normal_Desert", "ZmbM_usSoldier_normal_Woodland", "ZmbM_PatrolNormal_Winter", "ZmbF_usSoldier_normal_Desert", "ZmbF_usSoldier_normal_Woodland")
	"InfectedMedic"          = @("ZmbF_DoctorSkinny", "ZmbF_NurseFat", "ZmbF_ParamedicNormal_Blue", "ZmbF_ParamedicNormal_Green", "ZmbF_ParamedicNormal_Red", "ZmbF_PatientOld", "ZmbM_DoctorFat", "ZmbM_ParamedicNormal_Black", "ZmbM_ParamedicNormal_Blue", "ZmbM_ParamedicNormal_Green", "ZmbM_ParamedicNormal_Red", "ZmbM_PatientSkinny", "ZmbM_DoctorFat_Winter")
	"InfectedIndustrial"     = @("ZmbF_ClerkFat_Black", "ZmbF_ClerkFat_GreyPattern", "ZmbF_ClerkFat_BluePattern", "ZmbF_ClerkFat_White", "ZmbF_BlueCollarFat_Blue", "ZmbF_BlueCollarFat_Green", "ZmbF_BlueCollarFat_Red", "ZmbF_MechanicNormal_Beige", "ZmbF_MechanicNormal_Green", "ZmbF_MechanicNormal_Grey", "ZmbF_MechanicNormal_Orange", "ZmbM_ConstrWorkerNormal_Beige", "ZmbM_ConstrWorkerNormal_Black", "ZmbM_ConstrWorkerNormal_Green", "ZmbM_ConstrWorkerNormal_Grey", "ZmbM_HandymanNormal_Beige", "ZmbM_HandymanNormal_Blue", "ZmbM_HandymanNormal_Green", "ZmbM_HandymanNormal_Grey", "ZmbM_HandymanNormal_White", "ZmbM_HeavyIndustryWorker", "ZmbM_MechanicSkinny_Blue", "ZmbM_MechanicSkinny_Green", "ZmbM_MechanicSkinny_Grey", "ZmbM_MechanicSkinny_Red", "ZmbM_OffshoreWorker_Green", "ZmbM_OffshoreWorker_Orange", "ZmbM_OffshoreWorker_Red", "ZmbM_OffshoreWorker_Yellow")
	"InfectedFirefighter"    = @("ZmbM_FirefighterNormal")
	"InfectedPolice"         = @("ZmbF_PoliceWomanNormal", "ZmbM_PolicemanFat", "ZmbM_PolicemanSpecForce", "ZmbM_PolicemanSpecForce_Heavy")
	"InfectedNBC"            = @("ZmbM_NBC_Grey", "ZmbM_NBC_White")


}

# Список зон с ускоренной перереспавнивающимися классами (лайфайм 2400)
$vanillaZones = @(
    "InfectedVillage", "InfectedSolitude", "InfectedReligious", "InfectedPrisoner", "InfectedPolice",
    "InfectedMedic", "InfectedIndustrial", "InfectedFirefighter", "InfectedCity", "InfectedArmy"
)

# Создание содержимого для всех файлов
$results = @()

foreach ($inputFile in $inputFiles) {
    # Загрузка XML-файла
    [xml]$xmlContent = Get-Content -Path $inputFile

    # Обработка каждого узла <zone>
    $xmlContent.SelectNodes("//zone") | ForEach-Object {
        $zoneName = $_.name
        $smin = $_.smin
        $smax = $_.smax
        $dmin = $_.dmin
        $dmax = $_.dmax
        $x = [math]::Round([double]$_.x)
        $z = [math]::Round([double]$_.z)
        $r = [int]$_.r

        # Устанавливаем время переспавна для каждой категории
        $lifetime = if ($inputFile -eq "zombie_territories.xml") { 1900 } else { 5400 }

        # Определяем количество мутантов или зараженных в зависимости от файла
        $spawnAmount = if ($inputFile -eq "zombie_territories.xml") { $dmax } else { $smax }

        # Рассчитываем разброс от центра
        $r_25 = [math]::Round($r * 0.25)
        $r_minus_25 = -$r_25

        # Рандомная замена класснейма, если он есть в словаре
        $zoneClasses = @($zoneName)  # Изначально только один класснейм
        $zombieClasses = ""

        foreach ($key in $classReplacements.Keys) {
            if ($zoneName -match "^$key") {
                $zoneClasses = $classReplacements[$key]  # Замещаем на несколько классов
                if ($classReplacements.ContainsKey($zoneName)) {
                    $zombieClasses = $classReplacements[$zoneName] -join ","  # Получаем зомби только для зон с зомби
                }
                break
            }
        }

        # Формирование строки результата для третьей строки
        $spawnParams = "$spawnAmount|$lifetime|$r|$r_25|$r_minus_25"
        
        # Если для этой зоны есть зомби, выводим их
        if ($zombieClasses) {
            $results += "$zombieClasses`n$($x) 0 $($z)`n$spawnParams"
        }
        else {
            $results += "$($zoneClasses -join ",")`n$($x) 0 $($z)`n$spawnParams"
        }
    }
}

# Запись содержимого в файлы без изменения количества мутантов
foreach ($file in $outputFiles) {
    $modifiedResults = @()
    foreach ($result in $results) {
        # Просто записываем результат без изменений
        $modifiedResults += $result
    }

    # Записываем измененные результаты в файл
    $modifiedResults | Set-Content -Path $file
}

Write-Host "Обработка завершена. Создано 2 файла дла спавна."

Write-Host "Запускаем бат-файл после завершения основного скрипта"
Start-Process -FilePath "RUN_Add_1_to_night.bat" -Wait

Write-Host "Все файлы готовы и ночным увеличен +1 количество"