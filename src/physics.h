#include "component.h"

void resolve_collision(
    CDynamicCollision &p_col, const CBoundingBox &p_bbox, CTransform &p_xform,
    const std::map<float,
                   std::tuple<size_t, Overlap, sf::IntRect, CStaticCollision>>
        &static_overlaps) {
  int max_push = 32;
  float margin = 0.f;

  // Try to keep in each static collision info on which edge is supposed to work
  // as collider

  int count = 0;
  p_col.touchedGround = false;
  p_col.touchedCeiling = false;
  for (std::pair<float,
                 std::tuple<size_t, Overlap, sf::IntRect, CStaticCollision>>
           o : static_overlaps) {
    size_t id = std::get<0>(o.second);
    Overlap &overlap = std::get<1>(o.second);
    const sf::IntRect &rect = std::get<2>(o.second);
    const CStaticCollision &w_col = std::get<3>(o.second);

    if (p_col.prevOverlap.find(id) != p_col.prevOverlap.end()) {
      const Overlap &prevOverlap = p_col.prevOverlap[id];
      int count = 0;
      if (overlap.right >= 0 && overlap.left >= 0 && overlap.bottom >= 0 &&
          overlap.top >= 0) {
        // std::cout << "id: " << id << " Overlap: " << overlap.right << ":"
        // << overlap.left << ";" << overlap.bottom << ":" << overlap.top <<
        // "\n"; std::cout << "id: " << id << " PrevOverlap: " <<
        // prevOverlap.right << ":" << prevOverlap.left << ";" <<
        // prevOverlap.bottom << ":" << prevOverlap.top << "\n";
        if ((prevOverlap.bottom <= 0 && prevOverlap.top <= 0) &&
            (prevOverlap.right <= 0 && prevOverlap.left <= 0)) {
          // Previously no overlap at all, meaning we're intersecting
          // diagonally. Find out direction we're moving and act accordingly.
          if (p_xform.prevPos.y <= p_xform.pos.y &&
              !p_col.collidedThisFrame[StaticCollisionDirection::top] &&
              w_col.direction[StaticCollisionDirection::top]) {
            // Previous position was higher.
            // Just stand on the wall.
            if (std::abs(overlap.top) <= max_push) {
              std::cout << "Line 44 moving diagonally from above, previous No "
                           "Overlap, step on it "
                        << id << " let's move up for " << -overlap.top << "\n";
              p_xform.pos.y -= (overlap.top + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::top, true);
              p_col.touchedGround = true;
              // overlap.top = margin;
              ++count;
              // p_xform.skipPrevPosition = true;
            }
          } else if (p_xform.prevPos.y > p_xform.pos.y &&
                     !p_col.collidedThisFrame
                          [StaticCollisionDirection::bottom] &&
                     w_col.direction[StaticCollisionDirection::bottom]) {
            // Previous position was lower.
            // Bump head against the wall.
            if (std::abs(overlap.bottom) <= max_push) {
              std::cout << "Line 60 moving diagonally from below, previous No "
                           "Overlap, bump head "
                        << id << " let's move to the bottom for "
                        << overlap.bottom << "\n";
              p_xform.pos.y += (overlap.bottom + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::bottom,
                                          true);
              p_col.touchedCeiling = true;
              // overlap.bottom = margin;
              ++count;
              // p_xform.skipPrevPosition = true;
            }
          } else if (p_xform.prevPos.x > p_xform.pos.x &&
                     !p_col
                          .collidedThisFrame[StaticCollisionDirection::right] &&
                     w_col.direction[StaticCollisionDirection::right]) {
            // Previous position to the right
            // Get pushed to the right
            if (std::abs(overlap.right) <= max_push) {
              std::cout << "Line 77 Moving from right to left, previous No "
                           "Overlap, bump to the rignt "
                        << id << "let's move to the right for " << overlap.right
                        << "\n";
              p_xform.pos.x += (overlap.right + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::right,
                                          true);
              // overlap.right = margin;
              ++count;
              // p_xform.skipPrevPosition = true;
            }
          } else if (p_xform.prevPos.x <= p_xform.pos.x &&
                     !p_col.collidedThisFrame[StaticCollisionDirection::left] &&
                     w_col.direction[StaticCollisionDirection::left]) {
            // Previous position to the left
            // Get pushed to the left
            if (std::abs(overlap.left) <= max_push) {
              std::cout << "Line 94 Moving from left to right, previous No "
                           "Overlap, bump to the left "
                        << id << " let's move to the left for " << -overlap.left
                        << "\n";
              p_xform.pos.x -= (overlap.left + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::left, true);
              // overlap.left = margin;
              ++count;
              // p_xform.skipPrevPosition = true;
            }
          }
        } else if ((prevOverlap.left > 0 && prevOverlap.right > 0) &&
                   (prevOverlap.bottom <= 0 || prevOverlap.top <= 0)) {
          // Were moving vertically, on top of or below the wall
          // Definite horizontal overlap
          // Definite no vertical overlap
          // Means we step on the wall, or bump it with player's head
          if (p_xform.prevPos.x != p_xform.pos.x &&
              p_xform.prevPos.y > p_xform.pos.y &&
              !p_col.collidedThisFrame[StaticCollisionDirection::bottom] &&
              !p_col.collidedThisFrame[StaticCollisionDirection::left] &&
              !p_col.collidedThisFrame[StaticCollisionDirection::left] &&
              w_col.direction[StaticCollisionDirection::bottom] &&
              (w_col.direction[StaticCollisionDirection::left] ||
               w_col.direction[StaticCollisionDirection::right])) {
            // Diagonal movement from above
            std::cout << "Line 120 Moving diagonally from below\n";
            p_xform.pos.y += (overlap.bottom + margin);
            p_col.collidedThisFrame.set(StaticCollisionDirection::bottom, true);
          } else if (p_xform.prevPos.x != p_xform.pos.x &&
                     p_xform.prevPos.y < p_xform.pos.y &&
                     !p_col.collidedThisFrame[StaticCollisionDirection::top] &&
                     !p_col.collidedThisFrame[StaticCollisionDirection::left] &&
                     !p_col
                          .collidedThisFrame[StaticCollisionDirection::right] &&
                     w_col.direction[StaticCollisionDirection::top] &&
                     (w_col.direction[StaticCollisionDirection::left] ||
                      w_col.direction[StaticCollisionDirection::right])) {
            // Diagonal movement from above
            std::cout << "Line 131 Moving diagonally from above\n";
            p_xform.pos.y -= (overlap.top + margin);
            p_col.collidedThisFrame.set(StaticCollisionDirection::top, true);
          } else if (p_xform.prevPos.y > p_xform.pos.y &&
                     !p_col.collidedThisFrame
                          [StaticCollisionDirection::bottom] &&
                     w_col.direction[StaticCollisionDirection::bottom]) {
            // Moving from below, bumping head
            if (std::abs(overlap.bottom) <= max_push) {
              std::cout << "Line 143 Moving from below, prevoius Horizontal "
                           "Overlap, prevY:"
                        << p_xform.prevPos.y << ", currY:" << p_xform.pos.y
                        << " , bump head " << id << " let's move down for "
                        << overlap.bottom << "\n";
              p_xform.pos.y += (overlap.bottom + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::bottom,
                                          true);
              p_col.touchedCeiling = true;
              // overlap.bottom = margin;
              ++count;
              // p_xform.skipPrevPosition = true;
            }
          } else if (p_xform.prevPos.y <= p_xform.pos.y &&
                     !p_col.collidedThisFrame[StaticCollisionDirection::top] &&
                     w_col.direction[StaticCollisionDirection::top]) {
            // Moving from above, step on it
            if (std::abs(overlap.top) <= max_push) {
              std::cout << "Line 160 Moving from above previous Horizontal "
                           "Overlap, prevY:"
                        << p_xform.prevPos.y << ", currY:" << p_xform.pos.y
                        << " step on " << id << " let's move up for "
                        << -overlap.top << "\n";
              p_xform.pos.y -= (overlap.top + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::top, true);
              p_col.touchedGround = true;
              // overlap.top = margin;
              ++count;
              // p_xform.skipPrevPosition = true;
            }
          } else if (p_xform.prevPos.x >= p_xform.pos.x &&
                     !p_col
                          .collidedThisFrame[StaticCollisionDirection::right] &&
                     w_col.direction[StaticCollisionDirection::right]) {
            // Moving from the right, bump to the right
            if (std::abs(overlap.right) <= max_push) {
              std::cout << "Line 177 Moving from right to left previous "
                           "Horizontal Overlap, prevX:"
                        << p_xform.prevPos.x << ", currX:" << p_xform.pos.x
                        << " bump to the right " << id
                        << " let's move right for " << overlap.right << "\n";
              p_xform.pos.x += (overlap.right + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::right,
                                          true);
              ++count;
            }
          } else if (p_xform.prevPos.x <= p_xform.pos.x &&
                     !p_col.collidedThisFrame[StaticCollisionDirection::left] &&
                     w_col.direction[StaticCollisionDirection::left]) {
            // Moving from the left, bump to the left
            if (std::abs(overlap.left) <= max_push) {
              std::cout << "Line 192 Moving from left to right, previous "
                           "Horizontal Overlap, prevX:"
                        << p_xform.prevPos.x << ", currX:" << p_xform.pos.x
                        << " bump to the left " << id << " let's move left for "
                        << -overlap.left << "\n";
              p_xform.pos.x -= (overlap.left + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::left, true);
              ++count;
            }
          }
        } else if ((prevOverlap.bottom > 0 || prevOverlap.top > 0) &&
                   (prevOverlap.left <= 0 || prevOverlap.right <= 0)) {
          // Were overlapping vertically, level with the wall
          // Means we bump into it from left or right, depending on
          // direction of movement
          // if (p_xform.prevPos.y != p_xform.pos.y &&
          //     p_xform.prevPos.x > p_xform.pos.x &&
          //     !p_col.collidedThisFrame[StaticCollisionDirection::bottom] &&
          //     !p_col.collidedThisFrame[StaticCollisionDirection::top] &&
          //     !p_col.collidedThisFrame[StaticCollisionDirection::right] &&
          //     w_col.direction[StaticCollisionDirection::right] &&
          //     (w_col.direction[StaticCollisionDirection::bottom] ||
          //      w_col.direction[StaticCollisionDirection::top])) {
          //   // Moving diagonally from right
          //   if (std::abs(overlap.right) <= max_push) {
          //     std::cout << "Line 218 Moving diagonally from right\n";
          //     p_xform.pos.x += (overlap.right + margin);
          //     p_col.collidedThisFrame.set(StaticCollisionDirection::right,
          //                                 true);
          //   }
          // }
          // if (p_xform.prevPos.y != p_xform.pos.y &&
          //     p_xform.prevPos.x < p_xform.pos.x &&
          //     !p_col.collidedThisFrame[StaticCollisionDirection::bottom] &&
          //     !p_col.collidedThisFrame[StaticCollisionDirection::top] &&
          //     !p_col.collidedThisFrame[StaticCollisionDirection::left] &&
          //     w_col.direction[StaticCollisionDirection::left] &&
          //     (w_col.direction[StaticCollisionDirection::bottom] ||
          //      w_col.direction[StaticCollisionDirection::top])) {
          //   // Diagonal movement from above
          //   std::cout << "Line 232 Moving diagonally from left\n";
          //   p_xform.pos.x -= (overlap.left + margin);
          //   p_col.collidedThisFrame.set(StaticCollisionDirection::left,
          //   true);
          // }
          if (p_xform.prevPos.x > p_xform.pos.x &&
              !p_col.collidedThisFrame[StaticCollisionDirection::right] &&
              w_col.direction[StaticCollisionDirection::right]) {
            // Moving right to left
            // Bump and stay to the right of the wall
            if (std::abs(overlap.right) <= max_push) {
              std::cout << "Line 214 Moving from right to left, previous "
                           "Vertical Overlap, bump to the rignt "
                        << id << "let's move to the right for " << overlap.right
                        << "\n";
              p_xform.pos.x += (overlap.right + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::right,
                                          true);
              // overlap.right = margin;
              ++count;
              // p_xform.skipPrevPosition = true;
            }
          } else if (p_xform.prevPos.x <= p_xform.pos.x &&
                     !p_col.collidedThisFrame[StaticCollisionDirection::left] &&
                     w_col.direction[StaticCollisionDirection::left]) {
            // Moving left to right
            // Bump and stay to the left
            if (std::abs(overlap.left) <= max_push) {
              std::cout << "Line 232 Moving from left to right, previous "
                           "Vertical Overlap, bump to the left "
                        << id << " let's move to the left for " << -overlap.left
                        << "\n";
              p_xform.pos.x -= (overlap.left + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::left, true);
              // overlap.left = margin;
              ++count;
              // p_xform.skipPrevPosition = true;
            }
          } else if (p_xform.prevPos.y > p_xform.pos.y &&
                     !p_col.collidedThisFrame
                          [StaticCollisionDirection::bottom] &&
                     w_col.direction[StaticCollisionDirection::bottom]) {
            if (std::abs(overlap.bottom) <= max_push) {
              std::cout << "Line 246 Moving from below, previous Vertical "
                           "Overlap, bump head "
                        << id << " let's move down for " << overlap.bottom
                        << "\n";
              p_xform.pos.y += (overlap.bottom + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::bottom,
                                          true);
              p_col.touchedCeiling = true;
              ++count;
            }
          } else if (p_xform.prevPos.y <= p_xform.pos.y &&
                     !p_col.collidedThisFrame[StaticCollisionDirection::top] &&
                     w_col.direction[StaticCollisionDirection::top]) {
            if (std::abs(overlap.top) <= max_push) {
              std::cout << "Line 258 Moving from above, previous Vertical "
                           "Overlap, step on "
                        << id << " let's move up for " << -overlap.top << "\n";
              p_xform.pos.y -= (overlap.top + margin);
              p_col.collidedThisFrame.set(StaticCollisionDirection::top, true);
              p_col.touchedGround = true;
              ++count;
            }
          }
        }
      }

      // if (count > 0) {
      //   std::cout << "Collision proccesses this frame: " << count <<
      //   std::endl;
      // }
    }

    if (overlap.bottom < 0 || overlap.top < 0 || overlap.right < 0 ||
        overlap.left < 0) {
      p_col.prevOverlap[id] = overlap;
    }
    ++count;
    if (count >= p_col.maxColliders) {
      break;
    }
  }
  p_col.collidedThisFrame.reset();
}

// Collision resloution:
//   if ((overlap.right > 0 && overlap.left > 0) && (overlap.bottom > 0 &&
//   overlap.top > 0)) {
//   // Both horizontal and vertical overlap happened
//   if (p_col.prevOverlap.bottom > 0 || p_col.prevOverlap.top > 0) {
//     std::cout << "Previous vertical overlap\n";
//     if (p_xform.prevPos.x > p_xform.pos.x && p_xform.prevPos.y ==
//     p_xform.pos.y) { std::cout << "From right\n"; std::cout << "Overlap: " <<
//     overlap.right << ":" << overlap.left << ";" << overlap.bottom << ":" <<
//     overlap.top << "\n"; std::cout << "PrevOverlap: " <<
//     p_col.prevOverlap.right << ":" << p_col.prevOverlap.left << ";" <<
//     p_col.prevOverlap.bottom << ":" << p_col.prevOverlap.top << "\n";
//     std::cout << "prev xform: " << p_xform.prevPos.x << ":" <<
//     p_xform.prevPos.x << " current: " << p_xform.pos.x << ":" <<
//     p_xform.pos.y << "\n"; p_xform.pos.x += overlap.right; overlap.right = 0;
//     } else if (p_xform.prevPos.x < p_xform.pos.x && p_xform.prevPos.y ==
//     p_xform.pos.y) { std::cout << "From left\n"; std::cout << "Overlap: " <<
//     overlap.right << ":" << overlap.left << ";" << overlap.bottom << ":" <<
//     overlap.top << "\n"; std::cout << "PrevOverlap: " <<
//     p_col.prevOverlap.right << ":" << p_col.prevOverlap.left << ";" <<
//     p_col.prevOverlap.bottom << ":" << p_col.prevOverlap.top << "\n";
//     std::cout << "prev xform: " << p_xform.prevPos.x << ":" <<
//     p_xform.prevPos.x << " current: " << p_xform.pos.x << ":" <<
//     p_xform.pos.y << "\n"; p_xform.pos.x -= overlap.left; overlap.left = 0;
//     } else if (p_xform.prevPos.y > p_xform.pos.y) {
//     std::cout << "From diagonal bottom\n";
//     p_xform.pos.y += overlap.bottom;
//     overlap.bottom = 0;
//     } else if (p_xform.prevPos.y < p_xform.pos.y) {
//     std::cout << "From diagonal top\n";
//     p_xform.pos.y -= overlap.top;
//     overlap.top = 0;
//     }
//   }
//   if (p_col.prevOverlap.left > 0 || p_col.prevOverlap.right > 0 ) {
//     std::cout << "Prevous horizontal overlap\n";
//     if (p_xform.prevPos.y < p_xform.pos.y && p_xform.prevPos.x ==
//     p_xform.pos.x) { std::cout << "From top\n"; std::cout << "Overlap: " <<
//     overlap.right << ":" << overlap.left << ";" << overlap.bottom << ":" <<
//     overlap.top << "\n"; std::cout << "PrevOverlap: " <<
//     p_col.prevOverlap.right << ":" << p_col.prevOverlap.left << ";" <<
//     p_col.prevOverlap.bottom << ":" << p_col.prevOverlap.top << "\n";
//     std::cout << "prev xform: " << p_xform.prevPos.x << ":" <<
//     p_xform.prevPos.x << " current: " << p_xform.pos.x << ":" <<
//     p_xform.pos.y << "\n"; p_xform.pos.y -= overlap.top; overlap.top = 0; }
//     else if (p_xform.prevPos.y > p_xform.pos.y && p_xform.prevPos.x ==
//     p_xform.pos.x) { std::cout << "from bottom\n"; std::cout << "Overlap: "
//     << overlap.right << ":" << overlap.left << ";" << overlap.bottom << ":"
//     << overlap.top << "\n"; std::cout << "PrevOverlap: " <<
//     p_col.prevOverlap.right << ":" << p_col.prevOverlap.left << ";" <<
//     p_col.prevOverlap.bottom << ":" << p_col.prevOverlap.top << "\n";
//     std::cout << "prev xform: " << p_xform.prevPos.x << ":" <<
//     p_xform.prevPos.x << " current: " << p_xform.pos.x << ":" <<
//     p_xform.pos.y << "\n"; p_xform.pos.y += overlap.bottom; overlap.bottom =
//     0;
//     }
//   }
//   p_col.overlap = overlap;
//   }

// }
