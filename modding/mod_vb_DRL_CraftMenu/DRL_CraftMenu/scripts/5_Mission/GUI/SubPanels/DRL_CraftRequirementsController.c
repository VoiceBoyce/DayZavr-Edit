class DRL_CraftRequirementsController
{
    protected DRL_CraftMenuUI m_Menu;
    protected Widget m_Wrap;
    protected ScrollWidget m_Scroll;

    void DRL_CraftRequirementsController(DRL_CraftMenuUI menu, Widget wrap, ScrollWidget scroll = null)
    {
        m_Menu = menu;
        m_Wrap = wrap;
        m_Scroll = scroll;
    }

    DRL_CraftMenuUI GetMenu()
    {
        return m_Menu;
    }

    Widget GetWrap()
    {
        return m_Wrap;
    }

    void SetWidgets(Widget wrap, ScrollWidget scroll = null)
    {
        m_Wrap = wrap;
        m_Scroll = scroll;
    }

    void AddRequirementHeader(string title, string templatePath = "DRL_CraftMenu/UI/layout/RequirementHeader.layout")
    {
        if (!m_Wrap)
        {
            return;
        }

        Widget head = GetGame().GetWorkspace().CreateWidgets(templatePath, m_Wrap);
        if (head)
        {
            RichTextWidget txt = RichTextWidget.Cast(head.FindAnyWidget("RequirementHeaderText"));
            if (txt)
            {
                txt.SetText(title);
            }
        }
    }

    Widget AddRequirementItem(string title, string subtitle, string iconPath, string previewClass, bool isMet, string statusText = "", DRL_Craft_Recipe targetRecipe = null, string templatePath = "DRL_CraftMenu/UI/layout/RequirementItem.layout")
    {
        if (!m_Wrap)
        {
            return null;
        }

        Widget row = GetGame().GetWorkspace().CreateWidgets(templatePath, m_Wrap);
        if (!row)
        {
            return null;
        }

        RichTextWidget titleW = RichTextWidget.Cast(row.FindAnyWidget("ReqTitle"));
        if (titleW)
        {
            titleW.SetText(title);
        }

        TextWidget subW = TextWidget.Cast(row.FindAnyWidget("ReqSubtitle"));
        if (subW)
        {
            subW.SetText(subtitle);
        }

        ItemPreviewWidget prevW = ItemPreviewWidget.Cast(row.FindAnyWidget("ReqItemPreview"));
        ImageWidget iconW = ImageWidget.Cast(row.FindAnyWidget("ReqIcon"));

        if (previewClass != "" && prevW)
        {
            EntityAI prevEnt = m_Menu.GetPreviewEntity(previewClass);
            if (prevEnt)
            {
                DRL_CraftPreviewHelper.SetupPreviewWidget(prevW, prevEnt);
                m_Menu.AddAttachedPreview(prevW);
            }
        }
        else if (iconPath != "" && iconW)
        {
            iconW.LoadImageFile(0, iconPath);
            iconW.Show(true);
        }
        else if (iconW)
        {
            iconW.LoadImageFile(0, "DRL_CraftMenu/UI/data/info.edds");
            iconW.Show(true);
        }

        ImageWidget statusIcon = ImageWidget.Cast(row.FindAnyWidget("ReqStatusIcon"));
        TextWidget statusTxt = TextWidget.Cast(row.FindAnyWidget("ReqStatusText"));

        if (isMet)
        {
            if (statusIcon)
            {
                statusIcon.LoadImageFile(0, "DRL_CraftMenu/UI/data/baitinfo.edds");
                statusIcon.SetColor(ARGB(255, 80, 210, 80));
            }
            if (statusTxt)
            {
                if (statusText == "")
                {
                    statusText = "#STR_DRL_ATTACHMENT_INSTALLED";
                }
                statusTxt.SetText(statusText);
                statusTxt.SetColor(ARGB(255, 80, 210, 80));
            }
        }
        else
        {
            if (statusIcon)
            {
                statusIcon.LoadImageFile(0, "DRL_CraftMenu/UI/data/lock1.edds");
                statusIcon.SetColor(ARGB(255, 210, 70, 70));
            }
            if (statusTxt)
            {
                if (statusText == "")
                {
                    statusText = "#STR_DRL_ATTACHMENT_MISSING";
                }
                statusTxt.SetText(statusText);
                statusTxt.SetColor(ARGB(255, 210, 70, 70));
            }
        }

        ButtonWidget btn = ButtonWidget.Cast(row.FindAnyWidget("ReqButton"));
        if (btn && targetRecipe)
        {
            btn.SetUserData(targetRecipe);
            btn.SetUserID(targetRecipe.RecipeId);
        }

        return row;
    }

    void AddRequirementInfo(string text, int color = 0, string templatePath = "DRL_CraftMenu/UI/layout/RequirementInfo.layout")
    {
        if (!m_Wrap)
        {
            return;
        }

        Widget infoW = GetGame().GetWorkspace().CreateWidgets(templatePath, m_Wrap);
        if (!infoW)
        {
            return;
        }

        RichTextWidget t = RichTextWidget.Cast(infoW.FindAnyWidget("RequirementInfoText"));
        if (t)
        {
            t.SetText(text);
            if (color != 0)
            {
                t.SetColor(color);
            }
        }
    }

    bool AreAllRecipeRequirementsMet(DRL_Craft_Recipe r)
    {
        if (!r)
        {
            return true;
        }

        DRL_Learning_Config lcfg = GetLearning_Config();
        if (lcfg && lcfg.RecipeRequiresLearning(r.RecipeId))
        {
            if (!DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe(r.RecipeId))
            {
                return false;
            }
        }

        if (lcfg)
        {
            DRL_Learning_Recipe lr = lcfg.FindByPrimaryId(r.RecipeId);
            if (!lr)
            {
                lr = lcfg.FindByAnyRecipeId(r.RecipeId);
            }
            if (lr && lr.RequiredRecipeIds && lr.RequiredRecipeIds.Count() > 0)
            {
                for (int i = 0; i < lr.RequiredRecipeIds.Count(); i++)
                {
                    int reqId = lr.RequiredRecipeIds[i];
                    if (!DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe(reqId))
                    {
                        return false;
                    }
                }
            }
        }

        if (!m_Menu.DRL_ClientHasRequiredAttachments(r))
        {
            return false;
        }

        if (!m_Menu.CanCraftRecipe(r))
        {
            return false;
        }

        return true;
    }

    void BuildUI(DRL_Craft_Recipe r)
    {
        if (!m_Wrap)
        {
            return;
        }

        DRL_CraftUIHelper.ClearChildren(m_Wrap);

        if (!r)
        {
            return;
        }

        int totalReqCount = 0;
        DRL_Learning_Config lcfg = GetLearning_Config();

        bool recipeNeedsLearning = false;
        if (lcfg)
        {
            recipeNeedsLearning = lcfg.RecipeRequiresLearning(r.RecipeId);
        }

        DRL_Learning_Recipe lr = null;
        if (lcfg)
        {
            lr = lcfg.FindByPrimaryId(r.RecipeId);
            if (!lr)
            {
                lr = lcfg.FindByAnyRecipeId(r.RecipeId);
            }
        }

        bool hasPrereqs = false;
        if (lr && lr.RequiredRecipeIds && lr.RequiredRecipeIds.Count() > 0)
        {
            hasPrereqs = true;
        }

        if (recipeNeedsLearning || hasPrereqs)
        {
            AddRequirementHeader("#STR_Studying_Recipes");

            if (recipeNeedsLearning)
            {
                bool isLearned = DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe(r.RecipeId);
                string studyPrevCls = r.PreviewClassname;
                if (studyPrevCls == "")
                {
                    studyPrevCls = r.Result;
                }
                string studyStatusStr = "#STR_LearningLockTitel";
                if (isLearned)
                {
                    studyStatusStr = "#STR_DRL_LEARNED";
                }
                AddRequirementItem(r.RecipeName, "#STR_DRL_RECIPE_STUDY", "", studyPrevCls, isLearned, studyStatusStr);
                totalReqCount++;
            }

            if (hasPrereqs)
            {
                DRL_Craft_Config ccfg = GetCraft_Config();
                for (int pi = 0; pi < lr.RequiredRecipeIds.Count(); pi++)
                {
                    int prereqId = lr.RequiredRecipeIds[pi];
                    DRL_Craft_Recipe prereqR = null;
                    if (ccfg)
                    {
                        prereqR = ccfg.FindById(prereqId);
                    }
                    string pTitle = Widget.TranslateString("#STR_Recipe_name") + " #" + prereqId.ToString();
                    if (prereqR)
                    {
                        pTitle = prereqR.RecipeName;
                    }
                    string pPrev = "";
                    if (prereqR)
                    {
                        pPrev = prereqR.PreviewClassname;
                        if (pPrev == "")
                        {
                            pPrev = prereqR.Result;
                        }
                    }
                    bool pLearned = DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe(prereqId);
                    string pStatusStr = "#STR_RequiredToStudy";
                    if (pLearned)
                    {
                        pStatusStr = "#STR_DRL_LEARNED";
                    }

                    AddRequirementItem(pTitle, "#STR_DRL_PREREQUISITE_RECIPE", "", pPrev, pLearned, pStatusStr, prereqR);
                    totalReqCount++;
                }
            }
        }

        if (r.CraftComponents && r.CraftComponents.Count() > 0)
        {
            bool addedAttachHeader = false;
            string activeReqRes = m_Menu.GetSelectedResultClass();

            for (int ci = 0; ci < r.CraftComponents.Count(); ci++)
            {
                DRL_Craft_Component comp = r.CraftComponents[ci];
                if (!comp || comp.AttachmentsEx <= 0)
                {
                    continue;
                }

                if (!comp.IsApplicableForResult(activeReqRes))
                {
                    continue;
                }

                if (!addedAttachHeader)
                {
                    AddRequirementHeader("#STR_WB_TOOLS");
                    addedAttachHeader = true;
                }

                string token = comp.Classname;
                if (token == "" && comp.Classnames && comp.Classnames.Count() > 0)
                {
                    token = comp.Classnames[0];
                }
                PlayerBase pbReq = PlayerBase.Cast(GetGame().GetPlayer());
                EntityAI benchReq = m_Menu.GetContextWorkbench();
                bool okSlot = DRL_CraftComponentResolver.HasComponentRequirement(comp, pbReq, benchReq, activeReqRes, 1);

                string toolDisplayName = token;
                string cfgName;
                if (DRL_CraftUIHelper.GetDisplayNameForClass(token, cfgName) && cfgName != "")
                {
                    toolDisplayName = cfgName;
                }
                else if (InventorySlots.GetSlotIdFromString(token) > -1)
                {
                    toolDisplayName = InventorySlots.GetSlotDisplayName(InventorySlots.GetSlotIdFromString(token));
                }

                string attStatus = "#STR_DRL_ATTACHMENT_MISSING";
                if (okSlot)
                {
                    attStatus = "#STR_DRL_ATTACHMENT_INSTALLED";
                }
                AddRequirementItem(toolDisplayName, "#STR_DRL_WB_EQUIPMENT", "", token, okSlot, attStatus);
                totalReqCount++;
            }
        }

        if (totalReqCount == 0)
        {
            bool hasIngredients = m_Menu.CanCraftRecipe(r);

            if (!hasIngredients)
            {
                AddRequirementHeader("#STR_DRL_Craft_Ingridient_Titel");

                array<EntityAI> cachedItems = new array<EntityAI>();
                m_Menu.DRL_GetInventorySnapshot(cachedItems);
                int craftCount = Math.Max(1, r.CurrentCraftCount);
                bool addedAnyMissing = false;

                if (r == m_Menu.GetSelectedRecipe())
                {
                    array<ref DRL_CraftMergedComponent> mergedList = m_Menu.BuildMergedComponents(r);
                    for (int m = 0; m < mergedList.Count(); m++)
                    {
                        DRL_CraftMergedComponent mc = mergedList[m];
                        if (!mc)
                        {
                            continue;
                        }
                        DRL_Craft_Component cMerged = mc.ToCraftComponent();
                        if (!cMerged || cMerged.AttachmentsEx > 0)
                        {
                            continue;
                        }

                        string activeMergedCls = mc.Classname;
                        if (activeMergedCls == "")
                        {
                            activeMergedCls = cMerged.Classname;
                        }
                        int haveMerged = m_Menu.ComputeHaveForClass(activeMergedCls, cMerged.MinHealthFrac, cachedItems, cMerged.QuantityFrac, cMerged.UseInheritance == 1);
                        int needMerged = m_Menu.NeedForClassCount(activeMergedCls, cMerged, craftCount);
                        if (haveMerged < needMerged)
                        {
                            string mName = "";
                            DRL_CraftUIHelper.GetDisplayNameForClass(activeMergedCls, mName);
                            if (mName == "")
                            {
                                mName = activeMergedCls;
                            }

                            string mStatus = DRL_CraftInventoryHelper.FormatIngredientAmount(activeMergedCls, cMerged, haveMerged, needMerged, true);
                            string mSub = Widget.TranslateString("#STR_DRL_REQUIRED") + " " + DRL_CraftInventoryHelper.FormatIngredientNeed(activeMergedCls, cMerged, needMerged);
                            AddRequirementItem(mName, mSub, "", activeMergedCls, false, mStatus);
                            addedAnyMissing = true;
                        }
                    }
                }
                else if (r.CraftComponents)
                {
                    for (int mi = 0; mi < r.CraftComponents.Count(); mi++)
                    {
                        DRL_Craft_Component cIngr = r.CraftComponents[mi];
                        if (!cIngr || cIngr.AttachmentsEx > 0)
                        {
                            continue;
                        }

                        int haveQty = m_Menu.ComputeHaveForComponent(cIngr, mi, cachedItems);
                        int needQty = m_Menu.NeedForCount(cIngr, craftCount, mi);
                        if (haveQty < needQty)
                        {
                            string ingrName = "";
                            DRL_CraftUIHelper.GetDisplayNameForClass(cIngr.Classname, ingrName);
                            if (ingrName == "")
                            {
                                ingrName = cIngr.Classname;
                            }

                            string ingrStatus = DRL_CraftInventoryHelper.FormatIngredientAmount(cIngr.Classname, cIngr, haveQty, needQty, true);
                            string needSub = Widget.TranslateString("#STR_DRL_REQUIRED") + " " + DRL_CraftInventoryHelper.FormatIngredientNeed(cIngr.Classname, cIngr, needQty);
                            AddRequirementItem(ingrName, needSub, "", cIngr.Classname, false, ingrStatus);
                            addedAnyMissing = true;
                        }
                    }
                }

                if (!addedAnyMissing)
                {
                    AddRequirementItem("#STR_DRL_NOT_ENOUGH_INGREDIENTS", "#STR_DRL_CHECK_RECIPE_INGREDIENTS", "", "", false, "#STR_DRL_ATTACHMENT_MISSING");
                }
            }
            else
            {
                AddRequirementHeader("#STR_DRL_AdditionalTasks_Titel");
                AddRequirementItem("#STR_DRL_NO_ADDITIONAL_REQUIREMENTS", "#STR_DRL_READY_TO_CRAFT", "", "", true, "#STR_DRL_ATTACHMENT_INSTALLED");
            }
        }

        m_Wrap.Update();
        if (m_Scroll)
        {
            m_Scroll.Update();
        }
        m_Menu.UpdateUnderlyingOverlaysState();
    }
}
