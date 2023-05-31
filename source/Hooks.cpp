#include "Hooks.h"

namespace Address
{
	struct GetIngredients
	{
		static float thunk(RE::Effect* a_effect)
		{
			float highestMagnitude = a_effect->effectItem.magnitude;
			int highestDuration = a_effect->effectItem.duration;

			const auto ui = RE::UI::GetSingleton();
			auto alchemyMenu = static_cast<RE::CraftingSubMenus::AlchemyMenu*>(ui->GetMenu<RE::CraftingMenu>().get()->GetCraftingSubMenu());

			std::vector<RE::IngredientItem*> ingredients;

			ingredients.push_back(static_cast<RE::IngredientItem*>(alchemyMenu->ingredientsEntries[alchemyMenu->ingredientIndex1].ingredient->GetObject()));
			ingredients.push_back(static_cast<RE::IngredientItem*>(alchemyMenu->ingredientsEntries[alchemyMenu->ingredientIndex2].ingredient->GetObject()));
			
			if (alchemyMenu->selectedIngredientCount == 3) {
				ingredients.push_back(static_cast<RE::IngredientItem*>(alchemyMenu->ingredientsEntries[alchemyMenu->ingredientIndex3].ingredient->GetObject()));
			}

			for (unsigned short index = 0; auto ingredient : ingredients) {
				logs::info("Processing '{}' at index '{}' for '{}'.", ingredient->GetName(), index, a_effect->baseEffect->GetName());
				for (unsigned short i = 0; i < ingredient->GetMaxEffectCount(); i++) {
					if (ingredient->effects[i]->baseEffect == a_effect->baseEffect) {
						if (ingredient->effects[i]->GetMagnitude() > highestMagnitude) {
							logs::info("Ingredient '{}' has a higher magnitude of '{}' than the other ingredients. Updating magnitude from '{}' to '{}'.", ingredient->GetName(), ingredient->effects[i]->baseEffect->GetName(), highestMagnitude, ingredient->effects[i]->GetMagnitude());
							highestMagnitude = ingredient->effects[i]->GetMagnitude();
							highestDuration = ingredient->effects[i]->GetDuration();	
						} else { logs::info("Ingredient '{}' didn't provide any magnitude changes for '{}'.", ingredient->GetName(), a_effect->baseEffect->GetName()); }

						if (ingredient->effects[i]->GetMagnitude() == highestMagnitude) {
							if ((int)ingredient->effects[i]->GetDuration() > highestDuration) {
								logs::info("Ingredient '{}' has a higher duration of '{}' than the other ingredients. Updating duration from '{}' to '{}'.", ingredient->GetName(), ingredient->effects[i]->baseEffect->GetName(), highestDuration, ingredient->effects[i]->GetDuration());
								highestDuration = ingredient->effects[i]->GetDuration();
							} else { logs::info("Ingredient '{}' didn't provide any duration changes for '{}'.\n", ingredient->GetName(), a_effect->baseEffect->GetName()); }
						} else { logs::info("Ingredient '{}' didn't provide any duration changes for '{}'.\n", ingredient->GetName(), a_effect->baseEffect->GetName()); }
					}
				}
				index++;
			}
			a_effect->effectItem.magnitude = highestMagnitude;
			a_effect->effectItem.duration = highestDuration;

			return func(a_effect);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Hook()
	{
		logs::info("Hooked address(es).\n");
		REL::Relocation<std::uintptr_t> function{ RELOCATION_ID(50373, 51289), 0x10 };
		stl::write_thunk_call<GetIngredients>(function.address());
	}
}